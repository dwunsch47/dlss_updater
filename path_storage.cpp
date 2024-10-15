#include "path_storage.h"

#include <filesystem>
#include <exception>
#include <fstream>
#include <vector>
#include <map>

#if _DEBUG
#include <iostream>
#endif

using namespace std;

PathStorage::PathStorage() {
	restoreSavedFilePaths();
}

PathStorage::~PathStorage() {
	saveFilePaths();
}

void PathStorage::AddNewPaths(const vector<filesystem::path>& new_paths) {
	if (new_paths.empty()) {
		return;
	}

	is_changed = true;
	for (const filesystem::path& file_path : new_paths) {
		stored_path_to_recency.emplace(file_path, true);
	}

#if _DEBUG
	cout << "It added new paths. stored_paths size is " << stored_path_to_recency.size() << endl;
#endif
}

const map<filesystem::path, bool>& PathStorage::GetStoredPaths() const {
	return stored_path_to_recency;
}

void PathStorage::restoreSavedFilePaths() {
	if (!filesystem::exists(PATH_STORAGE_FILENAME)) {
		return;
	}
	fstream path_storage_file(PATH_STORAGE_FILENAME, ios::in);

	string file_path;

	while (getline(path_storage_file, file_path)) {
		stored_path_to_recency.emplace(file_path, false);
	}
#if _DEBUG
	cout << "We succesfully restored paths. Current number of paths is " << stored_path_to_recency.size() << endl;
#endif
}

void PathStorage::saveFilePaths() const {
	if (stored_path_to_recency.empty() || !is_changed) {
		return;
	}
	
	fstream storage_file(PATH_STORAGE_FILENAME, ios::out | ios::app);
	
	if (storage_file.good()) {
		for (const auto& [file_path, is_recent] : stored_path_to_recency) {
			if (is_recent) {
				storage_file << file_path.string() << '\n';
			}
		}
	}
	else {
		throw runtime_error("file cannot be opened"s);
	}
}
