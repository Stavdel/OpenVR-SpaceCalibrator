#include "Configuration.h"

#include "picojson.h"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>


static picojson::array FloatArray(const float *buf, int numFloats)
{
	picojson::array arr;

	for (int i = 0; i < numFloats; i++)
		arr.push_back(picojson::value(double(buf[i])));

	return arr;
}

static void LoadFloatArray(const picojson::value &obj, float *buf, int numFloats)
{
	if (!obj.is<picojson::array>())
		throw std::runtime_error("expected array, got " + obj.to_str());

	auto &arr = obj.get<picojson::array>();
	if (arr.size() != numFloats)
		throw std::runtime_error("wrong buffer size");

	for (int i = 0; i < numFloats; i++)
		buf[i] = (float) arr[i].get<double>();
}

static void ParseProfile(CalibrationContext &ctx, std::istream &stream)
{
	picojson::value v;
	std::string err = picojson::parse(v, stream);
	if (!err.empty())
		throw std::runtime_error(err);

	auto arr = v.get<picojson::array>();
	if (arr.size() < 1)
		throw std::runtime_error("no profiles in file");

	auto obj = arr[0].get<picojson::object>();

	ctx.referenceTrackingSystem = obj["reference_tracking_system"].get<std::string>();
	ctx.targetTrackingSystem = obj["target_tracking_system"].get<std::string>();
	ctx.calibratedRotation(0) = obj["roll"].get<double>();
	ctx.calibratedRotation(1) = obj["yaw"].get<double>();
	ctx.calibratedRotation(2) = obj["pitch"].get<double>();
	ctx.calibratedTranslation(0) = obj["x"].get<double>();
	ctx.calibratedTranslation(1) = obj["y"].get<double>();
	ctx.calibratedTranslation(2) = obj["z"].get<double>();

	if (obj["scale"].is<double>())
		ctx.calibratedScale = obj["scale"].get<double>();
	else
		ctx.calibratedScale = 1.0;

	if (obj["calibration_speed"].is<double>())
		ctx.calibrationSpeed = (CalibrationContext::Speed)(int) obj["calibration_speed"].get<double>();

	// Load continuous calibration fields
	if (obj["relative_pos_calibrated"].is<bool>()) {
		ctx.relativePosCalibrated = obj["relative_pos_calibrated"].get<bool>();
	}
	if (obj["lock_relative_position"].is<bool>()) {
		ctx.lockRelativePosition = obj["lock_relative_position"].get<bool>();
	}

	// Load relative transform (refToTargetPose)
	if (obj["relative_transform"].is<picojson::object>()) {
		auto refToTarget = obj["relative_transform"].get<picojson::object>();
		Eigen::Vector3d refToTargetTranslation(
			refToTarget["x"].get<double>(),
			refToTarget["y"].get<double>(),
			refToTarget["z"].get<double>()
		);

		Eigen::Vector3d refToTargetRotation(
			refToTarget["roll"].get<double>(),
			refToTarget["yaw"].get<double>(),
			refToTarget["pitch"].get<double>()
		);

		Eigen::Matrix3d rotationMatrix = (
			Eigen::AngleAxisd(refToTargetRotation[0], Eigen::Vector3d::UnitX()) *
			Eigen::AngleAxisd(refToTargetRotation[1], Eigen::Vector3d::UnitY()) *
			Eigen::AngleAxisd(refToTargetRotation[2], Eigen::Vector3d::UnitZ())
		).toRotationMatrix();

		ctx.refToTargetPose = Eigen::AffineCompact3d::Identity();
		ctx.refToTargetPose.linear() = rotationMatrix;
		ctx.refToTargetPose.translation() = refToTargetTranslation;
	}

	if (obj["chaperone"].is<picojson::object>())
	{
		auto chaperone = obj["chaperone"].get<picojson::object>();
		ctx.chaperone.autoApply = chaperone["auto_apply"].get<bool>();

		LoadFloatArray(chaperone["play_space_size"], ctx.chaperone.playSpaceSize.v, 2);

		LoadFloatArray(
			chaperone["standing_center"],
			(float *) ctx.chaperone.standingCenter.m,
			sizeof(ctx.chaperone.standingCenter.m) / sizeof(float)
		);

		if (!chaperone["geometry"].is<picojson::array>())
			throw std::runtime_error("chaperone geometry is not an array");

		auto &geometry = chaperone["geometry"].get<picojson::array>();

		if (geometry.size() > 0)
		{
			ctx.chaperone.geometry.resize(geometry.size() * sizeof(float) / sizeof(ctx.chaperone.geometry[0]));
			LoadFloatArray(chaperone["geometry"], (float *) ctx.chaperone.geometry.data(), geometry.size());

			ctx.chaperone.valid = true;
		}
	}

	ctx.validProfile = true;
}

static void WriteProfile(CalibrationContext &ctx, std::ostream &out)
{
	if (!ctx.validProfile)
		return;

	picojson::object profile;
	profile["reference_tracking_system"].set<std::string>(ctx.referenceTrackingSystem);
	profile["target_tracking_system"].set<std::string>(ctx.targetTrackingSystem);
	profile["roll"].set<double>(ctx.calibratedRotation(0));
	profile["yaw"].set<double>(ctx.calibratedRotation(1));
	profile["pitch"].set<double>(ctx.calibratedRotation(2));
	profile["x"].set<double>(ctx.calibratedTranslation(0));
	profile["y"].set<double>(ctx.calibratedTranslation(1));
	profile["z"].set<double>(ctx.calibratedTranslation(2));
	profile["scale"].set<double>(ctx.calibratedScale);

	double speed = (int) ctx.calibrationSpeed;
	profile["calibration_speed"].set<double>(speed);

	// Save continuous calibration fields
	Eigen::Vector3d refToTargetRotation = ctx.refToTargetPose.rotation().eulerAngles(0, 1, 2);
	Eigen::Vector3d refToTargetTranslation = ctx.refToTargetPose.translation();
	picojson::object refToTarget;
	refToTarget["x"].set<double>(refToTargetTranslation(0));
	refToTarget["y"].set<double>(refToTargetTranslation(1));
	refToTarget["z"].set<double>(refToTargetTranslation(2));
	refToTarget["roll"].set<double>(refToTargetRotation(0));
	refToTarget["yaw"].set<double>(refToTargetRotation(1));
	refToTarget["pitch"].set<double>(refToTargetRotation(2));
	profile["relative_pos_calibrated"].set<bool>(ctx.relativePosCalibrated);
	profile["lock_relative_position"].set<bool>(ctx.lockRelativePosition);
	profile["relative_transform"].set<picojson::object>(refToTarget);

	if (ctx.chaperone.valid)
	{
		picojson::object chaperone;
		chaperone["auto_apply"].set<bool>(ctx.chaperone.autoApply);
		chaperone["play_space_size"].set<picojson::array>(FloatArray(ctx.chaperone.playSpaceSize.v, 2));

		chaperone["standing_center"].set<picojson::array>(FloatArray(
			(float *) ctx.chaperone.standingCenter.m,
			sizeof(ctx.chaperone.standingCenter.m) / sizeof(float)
		));

		chaperone["geometry"].set<picojson::array>(FloatArray(
			(float *) ctx.chaperone.geometry.data(),
			sizeof(ctx.chaperone.geometry[0]) / sizeof(float) * ctx.chaperone.geometry.size()
		));

		profile["chaperone"].set<picojson::object>(chaperone);
	}

	picojson::value profileV;
	profileV.set<picojson::object>(profile);

	picojson::array profiles;
	profiles.push_back(profileV);

	picojson::value profilesV;
	profilesV.set<picojson::array>(profiles);

	out << profilesV.serialize(true);
}

static std::string ReadRegistryKey()
{
	FILE* config = fopen((std::string(getenv("HOME")) + "/.config/OpenVR-SpaceCalibrator/config.json").c_str(), "r");
	if (config == nullptr) {
		std::cout << "Could not find config file" << std::endl;
		return "";
	}
	if (fseek(config, 0, SEEK_END) == -1) {
		std::cout << "Could not seek to the end of the config file" << std::endl;
		return "";
	}
	long fileSize = ftell(config);
	if (fileSize == -1) {
		std::cout << "Could not get the size of the config file" << std::endl;
		return "";
	}
	rewind(config);
	std::string contents;
	contents.resize(fileSize);
	size_t result = fread(contents.data(), fileSize, 1, config);
	fclose(config);
	if (result == 0) {
		std::cout << "Error occurred when reading config file" << std::endl;
		return "";
	}
	return contents;
}

static void WriteRegistryKey(std::string contents)
{
	FILE* config = fopen((std::string(getenv("HOME")) + "/.config/OpenVR-SpaceCalibrator/config.json").c_str(), "w");
	if (config == nullptr) {
		std::cout << "Could not find config file" << std::endl;
		return;
	}
	size_t result = fwrite(contents.data(), contents.size(), 1, config);
	fclose(config);
	if (result == 0) {
		std::cout << "Error occurred when reading config file" << std::endl;
	}
}

void LoadProfile(CalibrationContext &ctx)
{
	ctx.validProfile = false;

	auto str = ReadRegistryKey();
	if (str == "")
	{
		std::cout << "Profile is empty" << std::endl;
		ctx.Clear();
		return;
	}

	try
	{
		std::stringstream io(str);
		ParseProfile(ctx, io);
		std::cout << "Loaded profile" << std::endl;
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "Error loading profile: " << e.what() << std::endl;
	}
}

void SaveProfile(CalibrationContext &ctx)
{
	std::cout << "Saving profile to registry" << std::endl;

	std::stringstream io;
	WriteProfile(ctx, io);
	WriteRegistryKey(io.str());
}
