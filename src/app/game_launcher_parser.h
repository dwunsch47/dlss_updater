#pragma once

#include "file_utilities.h"

#include <filesystem>
#include <vector>
#include <string>

namespace glparse {
	enum GameLauncher {
		STEAM,
		EGS
	};

	struct GameLauncherData {
		std::wstring reg_path;
		std::wstring reg_value;
	};

	std::vector<std::filesystem::path> parseLauncherPaths();
}

