#include "file_copy.h"
#include "path_storage.h"

#include <iostream>
#include <filesystem>
#include <vector>

#if _DEBUG
#include "tests.h"
#endif

int main(int argc, char* argv[]) {
	/*if (argc < 2) {
		return 1;
	}*/

	const std::string mode = argv[1];
	std::vector<std::filesystem::path> arguments;

	if (argc > 2) {
		for (int i = 2; i < argc; ++i) {
			arguments.push_back(argv[i]);
		}
	}

#if _DEBUG
	if (mode == "test") {
		TestPathStorage();
		return 1;
	}
#endif

	PathStorage current_storage;

	if (mode == "add") {
		current_storage.AddNewPaths(arguments);
	}
	else if (mode == "path_to_dll") {

	}
	else if (mode == "update") {
		// TODO: add options to --force-update even is installed version is newer
		fileCopy(current_storage.GetStoredPaths(), current_storage.GetDLLPath());
	}
	// TODO: add option to --force-keep old, obsolete paths
	else if (mode == "remove") {
		current_storage.RemovePaths(arguments);
	}
}