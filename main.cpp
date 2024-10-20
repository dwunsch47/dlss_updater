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

	PathStorage current_storage;

#if _DEBUG
	if (mode == "test") {
		TestPathStorage();
		return 1;
	}
#endif

	if (mode == "add") {
		std::vector<std::filesystem::path> new_file_paths;
		for (int i = 2; i < argc; ++i) {
			new_file_paths.push_back(argv[i]);
		}

		current_storage.AddNewPaths(new_file_paths);
	}
	else if (mode == "update") {
		fileCopy(current_storage.GetStoredPaths());
	}
}