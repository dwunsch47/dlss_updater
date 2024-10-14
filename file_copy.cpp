#include "file_copy.h"

#include <filesystem>
#include <vector>

using namespace std;

void fileCopy(const vector<filesystem::path>& paths) {

	const auto copyOptions = filesystem::copy_options::overwrite_existing;

	for (const filesystem::path& destination : paths) {
		filesystem::copy("nvngx.dll", destination, copyOptions);
	}
}
