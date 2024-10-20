#pragma once

#include <filesystem>
#include <string>
#include <tuple>

const std::string PATH_STORAGE_FILENAME = "dll_locations.txt";
const std::string DLSS_DLL_NAME = "nvngx_dlss.dll";
const std::string PROGRAM_NAME = "DLSS Updater";

namespace fileUtil {
	std::string getDLLVersion(const std::filesystem::path& file_path);
	std::tuple<int, int, int, int> formatDLLVersion(const std::string& file_version);
}
