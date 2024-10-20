#include "file_copy.h"
#include "file_utilities.h"

#include <filesystem>
#include <algorithm>
#include <string>
#include <tuple>
#include <map>

using namespace std;

void fileCopy(const map<filesystem::path, std::tuple<bool, std::string>>& paths, const filesystem::path& dll_path) {
	const tuple<int, int, int, int> dll_version = fileUtil::formatDLLVersion(fileUtil::getDLLVersion(dll_path / DLSS_DLL_NAME));
	const uintmax_t dll_size = filesystem::file_size(dll_path / DLSS_DLL_NAME);

	const auto copyOptions = filesystem::copy_options::overwrite_existing;

	tuple<int, int, int, int> file_path_version = { 0, 0, 0, 0 };
	for (const auto& [file_path, info] : paths) {
		file_path_version = fileUtil::formatDLLVersion(fileUtil::getDLLVersion(file_path / DLSS_DLL_NAME));

		if (dll_version > file_path_version || (dll_version == file_path_version && dll_size != filesystem::file_size(file_path / DLSS_DLL_NAME))) {
			filesystem::copy(DLSS_DLL_NAME, file_path, copyOptions);
		}
	}
}
