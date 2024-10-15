#include "file_copy.h"

#include <filesystem>
#include <map>

using namespace std;

void fileCopy(const map<filesystem::path, bool>& paths) {

	const auto copyOptions = filesystem::copy_options::overwrite_existing;

	for (const auto& [file_path, recency] : paths) {
		filesystem::copy("nvngx.dll", file_path, copyOptions);
	}
}
