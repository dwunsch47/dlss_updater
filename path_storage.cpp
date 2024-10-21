#include "path_storage.h"
#include "file_utilities.h"

#include <filesystem>
#include <exception>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
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

	new_paths_added_ = true;

	for (filesystem::path file_path : new_paths) {
		if (!filesystem::exists(file_path) && !filesystem::exists(file_path.parent_path()) && (file_path == filesystem::current_path())) {
			continue;
		}

		if (filesystem::is_regular_file(file_path)) {
			file_path = file_path.parent_path();
		}
		else if (!filesystem::is_directory(file_path)) {
			continue;
		}

		if (filesystem::exists(file_path / DLSS_DLL_NAME)) {
			stored_path_to_recency_version_.emplace(file_path, make_tuple(true, fileUtil::getDLLVersion(file_path / DLSS_DLL_NAME)));
		}
		else {
			filesystem::recursive_directory_iterator recur_file_path_it(file_path);
			for (const filesystem::path& true_file_path : recur_file_path_it) {
				if (filesystem::is_regular_file(true_file_path) && true_file_path.filename() == DLSS_DLL_NAME) {
					stored_path_to_recency_version_.emplace(true_file_path.parent_path(), make_tuple(true, fileUtil::getDLLVersion(true_file_path)));
					recur_file_path_it.pop();
					recur_file_path_it.disable_recursion_pending();
				}
			}
		}
	}

#if _DEBUG
	cout << "It added new paths. stored_paths size is " << stored_path_to_recency_version_.size() << endl;
	/*cout << "Displaying contents of stored_path_to_recency_:";
	for (const auto& [curr_disp_path, recency] : stored_path_to_recency_) {
		cout << ' ' << curr_disp_path.string();
	}
	cout << endl;*/
#endif
}

void PathStorage::RemovePaths(const vector<filesystem::path>& paths_for_removal) {
	if (paths_for_removal.empty()) {
		return;
	}

	are_any_lines_for_deletion_ = true;
	for (const filesystem::path& file_path : paths_for_removal) {
		if (stored_path_to_recency_version_.find(file_path) != stored_path_to_recency_version_.end()) {
			stored_path_to_recency_version_.erase(file_path);
		}
	}
}

const map<filesystem::path, tuple<bool, string>>& PathStorage::GetStoredPaths() const {
	return stored_path_to_recency_version_;
}

const std::filesystem::path& PathStorage::GetConfigDirPath() const {
	return config_dir_location_;
}

void PathStorage::restoreSavedFilePaths() {
	if (!filesystem::exists(config_dir_location_ / PATH_STORAGE_FILENAME)) {
		return;
	}

	fstream path_storage_file(config_dir_location_ / PATH_STORAGE_FILENAME, ios::in);
	string tmp_str;

	if (!getline(path_storage_file, tmp_str) || tmp_str != PROGRAM_NAME) {
		return;
	}
	is_config_properly_formatted_ = true;

	filesystem::path file_path;

	while (getline(path_storage_file, tmp_str)) {
		file_path = tmp_str;
		if (filesystem::exists(file_path / DLSS_DLL_NAME)) {
			stored_path_to_recency_version_.emplace(file_path, make_tuple(false, fileUtil::getDLLVersion(file_path / DLSS_DLL_NAME)));
		}
		else {
			are_any_lines_for_deletion_ = true;
		}
	}
#if _DEBUG
	cout << "We succesfully restored paths. Current number of paths is " << stored_path_to_recency_version_.size() << endl;
#endif
}

void PathStorage::saveFilePaths() const {
	if (stored_path_to_recency_version_.empty() || (!new_paths_added_ && !are_any_lines_for_deletion_)) {
		return;
	}

	cout << "Are there line for deletion? " << are_any_lines_for_deletion_ << endl;

	const auto fstream_options = (are_any_lines_for_deletion_ ? ios::out : (ios::out | ios::app));
	
	fstream storage_file(config_dir_location_ / PATH_STORAGE_FILENAME, fstream_options);
	if (!storage_file.good()) {
		throw runtime_error("file cannot be opened"s);
	}
#if _DEBUG
	cout << "Storage file was opened" << endl;
#endif

	if (filesystem::is_empty(config_dir_location_ / PATH_STORAGE_FILENAME)) {
		storage_file << PROGRAM_NAME << '\n';
	}
	else if (!is_config_properly_formatted_) {
		return;
	}

	if (are_any_lines_for_deletion_) {
		for (const auto& [file_path, info] : stored_path_to_recency_version_) {
			if (!get<0>(info)) { // at this stage restore only previously stored paths to keep right order
				storage_file << file_path.string() << '\n';
			}
		}
	}

	if (new_paths_added_) {
		for (const auto& [file_path, info] : stored_path_to_recency_version_) {
			if (get<0>(info)) { // check whether current path is recent and wasn't saved before
				storage_file << file_path.string() << '\n';
			}
		}
	}
#if _DEBUG
	cout << "Paths were stored succesfully" << endl;
#endif
}