#include "game_launcher_parser.h"
#include "file_utilities.h"

#include <unordered_map>
#include <filesystem>
#include <vector>
#include <fstream>

#include <json.hpp>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <vdf_parser.hpp>
#pragma warning(pop)

using namespace std;

namespace glparse {
	vector<filesystem::path> parseLauncherPaths() {
		// Yes, it's shit and should not be done like this
		const unordered_map<GameLauncher, GameLauncherData> launcher_name_to_data = {
			{ GameLauncher::STEAM, { L"SOFTWARE\\Wow6432Node\\Valve\\Steam", L"InstallPath" } },
			{ GameLauncher::EGS, { L"SOFTWARE\\WOW6432Node\\Epic Games\\EpicGamesLauncher", L"AppDataPath" }},
			{ GameLauncher::GOG, { L"SOFTWARE\\WOW6432Node\\GOG.com\\Games", L"path"}}
		};

		vector<filesystem::path> result;
		vector<filesystem::path> tmp;

		for (const auto& [name, data] : launcher_name_to_data) {
			switch (name) {
				case GameLauncher::STEAM:
					tmp = parseVdf(data);
					break;
				case GameLauncher::EGS:
					tmp = parseEgsManifests(data);
					break;
				case GameLauncher::GOG:
					tmp = parseGOGReg(data);
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

namespace {
	vector<filesystem::path> parseVdf(const GameLauncherData& l_data) {
		const filesystem::path file_path = fileUtil::getPathFromRegistry(l_data.reg_path, l_data.reg_value);
		const filesystem::path vdf_path = file_path / STEAM_LIBRARYFOLDERS_PATH;
		if (!filesystem::exists(vdf_path)) {
			return {};
		}
		std::ifstream vdf_file(vdf_path);
		auto root = tyti::vdf::read(vdf_file);
		vdf_file.close();
		if (root.name == "libraryfolders") {
			vector<filesystem::path> result;
			result.reserve(root.childs.size());
			filesystem::path tmp_path;
			for (const auto& [field_name, ptr] : root.childs) {
				tmp_path = ptr->attribs["path"];
				result.push_back(tmp_path / STEAM_GAMES_PATH_POSTFIX);
			}
			return result;
		}
		else {
			return {};
		}
	}

	vector<filesystem::path> parseEgsManifests(const GameLauncherData& l_data) {
		vector<filesystem::path> result;
		const filesystem::path file_path = fileUtil::getPathFromRegistry(l_data.reg_path, l_data.reg_value);
		for (const auto& parse_path : filesystem::directory_iterator(file_path / EGS_MANIFESTS_PATH)) {
			if (!filesystem::is_regular_file(parse_path)) {
				continue;
			}
			ifstream item_file(parse_path.path());
			nlohmann::json parsed_item = nlohmann::json::parse(item_file);
			item_file.close();
			if (parsed_item.find("InstallLocation") != parsed_item.end()) {
				result.push_back(parsed_item.at("InstallLocation"));
			}
		}
		return result;
	}

	vector<filesystem::path> parseGOGReg(const GameLauncherData& l_data) {
		vector<filesystem::path> result;
		vector<wstring> subkeys = fileUtil::getSubkeys(l_data.reg_path);
		if (subkeys.empty()) {
			return result;
		}
		wstring full_subkey;
		for (const wstring& sk : subkeys) {
			full_subkey = l_data.reg_path + L'\\' + sk;
			result.emplace_back(fileUtil::getPathFromRegistry(full_subkey, l_data.reg_value));
		}
		return result;
	}
} // namespace
} // namespace glparse

