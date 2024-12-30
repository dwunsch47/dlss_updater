#pragma once

#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

const std::string PATH_STORAGE_FILENAME = "dll_locations.toml";
const std::string DLSS_DLL_NAME = "nvngx_dlss.dll";
const std::string PROGRAM_NAME = "DLSS Updater";
const std::string STEAM_GAMES_PATH_POSTFIX = "steamapps\\common";
const std::string STEAM_LIBRARYFOLDERS_PATH = "steamapps\\libraryfolders.vdf";
const std::string EGS_MANIFESTS_PATH = "Manifests";

namespace fileUtil {
	std::string getDLLVersion(const std::filesystem::path& file_path);
	std::tuple<int, int, int, int> formatDLLVersion(const std::string& file_version);

	std::string getPathFromRegistry(const std::wstring& reg_path, const std::wstring& reg_value);
	std::vector<std::wstring> getSubkeys(const std::wstring& reg_path);
} // namespace fileUtil
