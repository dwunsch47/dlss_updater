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

#include <toml.hpp>

using namespace std;

PathStorage::PathStorage() {
	restoreSavedFilePaths();
}

PathStorage::PathStorage(filesystem::path path_to_dll) 
	: dll_dir_path_(filesystem::is_directory(path_to_dll) ? path_to_dll : path_to_dll.parent_path()) {
	restoreSavedFilePaths();
}

PathStorage::~PathStorage() {
	savePathsAndConfig();
}

void PathStorage::AddNewPaths(const vector<filesystem::path>& new_paths) {
	if (new_paths.empty()) {
		return;
	}

	new_paths_added_ = true;

	vector<future<void>> dir_futur;

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
	/*cout << "Displaying contents of stored_path_to_recency_:";
	for (const auto& [curr_disp_path, recency] : stored_path_to_recency_) {
		cout << ' ' << curr_disp_path.string();
	}
	cout << endl;*/
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
			stored_paths_.erase(file_path);
		}
	}
}

void PathStorage::ScanForGameServices() {
	scanSteamFolder();
}

void PathStorage::scanSteamFolder() {
	vector<filesystem::path> steam_folders = fileUtil::parseVdf(fileUtil::getPathFromRegistry(launcher_name_to_data_.at("steam").reg_path, launcher_name_to_data_.at("steam").reg_value));
	if (steam_folders.empty()) {
		return;
	}
	AddNewPaths(steam_folders);
}

const unordered_set<filesystem::path>& PathStorage::GetStoredPaths() const {
	return stored_paths_;
}

std::filesystem::path PathStorage::GetDLLPath() const {
	return dll_dir_path_;
}

void PathStorage::ShowStoredDllsVersions() const {
	if (stored_paths_.empty()) {
		cout << "No game folders with DLSS were added. Use \"scan\" or \"add\" to add folders";
		return;
	}
	if (filesystem::exists(dll_dir_path_ / DLSS_DLL_NAME)) {
		cout << "Latest available DLSS version is: " << fileUtil::getDLLVersion(dll_dir_path_ / DLSS_DLL_NAME) << '\n';
	}

	filesystem::path full_dll_path;
	for (const auto& file_path : stored_paths_) {
		full_dll_path = file_path / DLSS_DLL_NAME;
		if (filesystem::exists(full_dll_path)) {;
			cout << '"' << full_dll_path.generic_string() << "\", version: " << fileUtil::getDLLVersion(full_dll_path) << "\n";
		}
	}
}

void PathStorage::restoreSavedFilePaths() {
	if (!filesystem::exists(PATH_STORAGE_FILENAME)) {
		return;
	}

	const toml::value path_storage_file = toml::parse(PATH_STORAGE_FILENAME);

	if (path_storage_file.at("name").as_string() != PROGRAM_NAME) {
		return;
	}

	filesystem::path tmp_path;
	for (const auto& file_path : path_storage_file.at("dll_paths").as_array()) {
		tmp_path = file_path.as_string();
		if (filesystem::exists(tmp_path / DLSS_DLL_NAME)) {
			stored_paths_.emplace(tmp_path);
		}
	}

	
#if _DEBUG
	cout << "We succesfully restored paths. Current number of paths is " << stored_paths_.size() << endl;
#endif
}

void PathStorage::savePathsAndConfig() const {
	savePaths();
	//saveConfig();
}

void PathStorage::savePaths() const {
	if (stored_paths_.empty() || !new_paths_added_) {
		return;
	}

	fstream dll_path_storage_file(PATH_STORAGE_FILENAME, ios::out);
	if (!dll_path_storage_file.good()) {
		throw runtime_error(PATH_STORAGE_FILENAME + " cannot be opened");
	}

	toml::value config_toml;
	config_toml["name"] = PROGRAM_NAME;


	toml::value path_storage_toml(toml::array{});
	for (const auto& file_path : stored_paths_) {
		path_storage_toml.push_back(file_path.generic_string());
	}

	config_toml["dll_paths"] = path_storage_toml;

	dll_path_storage_file << toml::format(config_toml);

#if _DEBUG
	cout << "All paths were stored succesfully" << endl;
#endif
}