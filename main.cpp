#include "file_copy.h"
#include "path_storage.h"

#include <iostream>
#include <filesystem>
#include <vector>

#if _DEBUG
#include "tests.cpp"
#endif

int main(int argc, char* argv[]) {
	/*if (argc < 2) {
		return 1;
	}*/

#if _DEBUG
	
#endif

	std::vector<std::filesystem::path> new_file_paths;
	for (int i = 1; i < argc; ++i) {
		new_file_paths.push_back(argv[i]);
	}

	PathStorage current_storage;
	current_storage.AddNewPaths(new_file_paths);

	fileCopy(current_storage.GetStoredPaths());
}