#include "file_copy.h"

#include <filesystem>
#include <tuple>
#include <map>

using namespace std;

void fileCopy(const map<filesystem::path, std::tuple<bool, std::string>>& paths) {

	const auto copyOptions = filesystem::copy_options::overwrite_existing;

	for (const auto& [file_path, info] : paths) {
		filesystem::copy("nvngx_dlss.dll", file_path, copyOptions);
	}
}
