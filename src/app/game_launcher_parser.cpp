#include "game_launcher_parser.h"
#include "file_utilities.h"

#include <unordered_map>
#include <filesystem>
#include <vector>

using namespace std;

namespace glparse {
	vector<filesystem::path> parseLauncherPaths() {
		// Yes, it's shit and shoult not be done like this
		unordered_map<GameLauncher, GameLauncherData> launcher_name_to_data = {
			{ GameLauncher::STEAM, { L"SOFTWARE\\Wow6432Node\\Valve\\Steam", L"InstallPath" } },
			{ GameLauncher::EGS, { L"SOFTWARE\\WOW6432Node\\Epic Games\\EpicGamesLauncher", L"AppDataPath" }}
		};

		vector<filesystem::path> result;
		filesystem::path file_to_parse;
		vector<filesystem::path> tmp;

		for (const auto& [name, data] : launcher_name_to_data) {
			file_to_parse = fileUtil::getPathFromRegistry(data.reg_path, data.reg_value);
			if (file_to_parse.empty()) {
				continue;
			}
			switch (name) {
			case GameLauncher::STEAM:
				tmp = fileUtil::parsers::parseVdf(file_to_parse);
				break;
			case GameLauncher::EGS:
				tmp = fileUtil::parsers::parseIndex(file_to_parse);
				break;
			}
			if (!tmp.empty()) {
				result.insert(
					result.end(),
					move_iterator(tmp.begin()),
					move_iterator(tmp.end()));
			}
		}
		return result;
	}
}

