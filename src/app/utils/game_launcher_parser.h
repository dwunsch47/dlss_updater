#pragma once

#include "file_utilities.h"

#include <filesystem>
#include <vector>
#include <string>

namespace glparse {
	enum GameLauncher {
		STEAM,
		EGS,
		GOG,
	};

	struct GameLauncherData {
		std::wstring reg_path;
		std::wstring reg_value;
	};

	std::vector<std::filesystem::path> parseLauncherPaths();

namespace {
	std::vector<std::filesystem::path> parseWrapper(GameLauncher g_l, const GameLauncherData& gl_d);

	std::vector<std::filesystem::path> parseVdf(const GameLauncherData& l_data);
	std::vector<std::filesystem::path> parseEgsManifests(const GameLauncherData& l_data);
	std::vector<std::filesystem::path> parseGOGReg(const GameLauncherData& l_data);
} // namespace
} // namespace glparse

