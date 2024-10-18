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

PathStorage::PathStorage(filesystem::path path_to_config_dir) 
	: config_dir_location_(filesystem::is_directory(path_to_config_dir) ? path_to_config_dir : path_to_config_dir.parent_path()) {
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
	/*for (const filesystem::path& file_path : new_paths) {
		stored_path_to_recency.emplace(file_path, true);
	}*/
#if _DEBUG
	int num_of_it_elems = 0;
#endif

	for (filesystem::path file_path : new_paths) {
		if (!filesystem::exists(file_path) && !filesystem::exists(file_path.parent_path()) && (file_path == filesystem::current_path())) {
			continue;
		}

		if (filesystem::is_regular_file(file_path)) {
			file_path = file_path.parent_path();
		}
		else if (!filesystem::is_directory(file_path)) {
			throw runtime_error("given path is neither a directory, nor a regular file: " + file_path.string());
		}

		if (filesystem::exists(file_path / DLSS_DLL_NAME)) {
			stored_path_to_recency_.emplace(file_path, true);
		}
		else {
			filesystem::recursive_directory_iterator recur_file_path_it(file_path);
			for (const filesystem::path& true_file_path : recur_file_path_it) {
#if _DEBUG
				//cout << "Current recur dir: " << recur_file_path_it->path().string() << endl;
				++num_of_it_elems;
#endif
				if (filesystem::is_regular_file(true_file_path) && true_file_path.filename() == DLSS_DLL_NAME) {
					stored_path_to_recency_.emplace(true_file_path.parent_path(), true);
					recur_file_path_it.pop();
					recur_file_path_it.disable_recursion_pending();
				}
			}
		}
	}

#if _DEBUG
	cout << "Number of iterated elements: " << num_of_it_elems << endl;
	cout << "It added new paths. stored_paths size is " << stored_path_to_recency_.size() << endl;
	/*cout << "Displaying contents of stored_path_to_recency_:";
	for (const auto& [curr_disp_path, recency] : stored_path_to_recency_) {
		cout << ' ' << curr_disp_path.string();
	}
	cout << endl;*/
#endif
}

const map<filesystem::path, bool>& PathStorage::GetStoredPaths() const {
	return stored_path_to_recency_;
}

void PathStorage::restoreSavedFilePaths() {
	if (!filesystem::exists(config_dir_location_ / PATH_STORAGE_FILENAME)) {
		return;
	}
	fstream path_storage_file(config_dir_location_ / PATH_STORAGE_FILENAME, ios::in);

	string file_path;

	while (getline(path_storage_file, file_path)) {
		stored_path_to_recency_.emplace(file_path, false);
	}
#if _DEBUG
	cout << "We succesfully restored paths. Current number of paths is " << stored_path_to_recency_.size() << endl;
#endif
}

void PathStorage::saveFilePaths() const {
	if (stored_path_to_recency_.empty() || !is_changed) {
		return;
	}
	
	fstream storage_file(config_dir_location_ / PATH_STORAGE_FILENAME, ios::out | ios::app);
	
	if (storage_file.good()) {
		for (const auto& [file_path, is_recent] : stored_path_to_recency_) {
			if (is_recent) {
				storage_file << file_path.string() << '\n';
			}
		}
	}
	else {
		throw runtime_error("file cannot be opened"s);
	}
}