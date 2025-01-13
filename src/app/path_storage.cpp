#include "path_storage.h"
#include "file_utilities.h"

#include <unordered_set>
#include <filesystem>
#include <exception>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <mutex>
#include <future>
#include <iostream>

using namespace std;

PathStorage::PathStorage(const vector<filesystem::path>& paths_to_restore) {
	restorePaths(paths_to_restore);
}

void PathStorage::AddNewPaths(const vector<filesystem::path>& new_paths) {
	if (new_paths.empty()) {
		return;
	}

	are_stored_paths_changed_ = true;

	vector<future<void>> dir_futur;

	for (filesystem::path file_path : new_paths) {
		if (!filesystem::exists(file_path) && !filesystem::exists(file_path.parent_path()) && (file_path == filesystem::current_path())) {
			continue;
		}

		//are_stored_paths_changed_ = true;

		if (filesystem::is_regular_file(file_path)) {
			file_path = file_path.parent_path();
		}
		else if (!filesystem::is_directory(file_path)) {
			continue;
		}

		if (filesystem::exists(file_path / DLSS_DLL_NAME)) {
			std::lock_guard stored_paths_lock(mutex_stored_paths_);
			stored_paths_.insert(file_path);
		}
		else {
			for (auto& curr_dir_path : filesystem::directory_iterator(file_path)) {
				dir_futur.push_back(async(launch::async, &PathStorage::checkDirectoryPath, this, curr_dir_path));
			}
		}
	}

	for (auto& task : dir_futur) {
		task.wait();
	}

#if _DEBUG
	cout << "It added new paths. stored_paths size is " << stored_paths_.size() << endl;
#endif
}

void PathStorage::checkDirectoryPath(const filesystem::path& dir_path) {
	filesystem::recursive_directory_iterator recur_file_path_it(dir_path);
	for (const filesystem::path& true_file_path : recur_file_path_it) {
		if (filesystem::is_regular_file(true_file_path) && true_file_path.filename() == DLSS_DLL_NAME) {
			std::lock_guard stored_paths_lock(mutex_stored_paths_);
			stored_paths_.insert(true_file_path.parent_path());
			recur_file_path_it.disable_recursion_pending();
		}
	}
}

void PathStorage::RemovePaths(const vector<filesystem::path>& paths_for_removal) {
	if (paths_for_removal.empty()) {
		return;
	}

	for (const filesystem::path& file_path : paths_for_removal) {
		if (stored_paths_.find(file_path) != stored_paths_.end()) {
			are_stored_paths_changed_ = true;
			stored_paths_.erase(file_path);
		}
	}
}

const unordered_set<filesystem::path>& PathStorage::GetStoredPaths() const {
	return stored_paths_;
}

bool PathStorage::ArePathsChanged() const {
	return are_stored_paths_changed_;
}

void PathStorage::restorePaths(vector<filesystem::path> paths_to_restore) {
	for (filesystem::path file_path : paths_to_restore) {
		if (filesystem::exists(file_path / DLSS_DLL_NAME)) {
			stored_paths_.insert(file_path);
		}
	}
	
#if _DEBUG
	cout << "We successfully restored paths. Current number of paths is " << stored_paths_.size() << endl;
#endif
}