#include "path_storage.h"
#include "file_utilities.h"
#include "game_launcher_parser.h"

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
	restorePathsAndConfig();
}

PathStorage::~PathStorage() {
	savePathsAndConfig();
}

void PathStorage::AddDllPath(const vector<filesystem::path>& dll_path) {
	if (dll_path.empty()) {
		return;
	}
	const filesystem::path new_dll_path = dll_path.at(0);
	if (filesystem::exists(new_dll_path) && (new_dll_path.filename() == DLSS_DLL_NAME)) {
		dll_dir_path_ = new_dll_path.parent_path();
	}
	else if (filesystem::exists(new_dll_path / DLSS_DLL_NAME)) {
		dll_dir_path_ = new_dll_path;
	}
	is_dll_dir_path_changed_ = true;
}

void PathStorage::RestoreDllPath() {
	dll_dir_path_ = filesystem::current_path();
	is_dll_dir_path_changed_ = false;
}

void PathStorage::AddNewPaths(const vector<filesystem::path>& new_paths) {
	if (new_paths.empty()) {
		return;
	}

	stored_paths_changed_ = true;

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
			stored_paths_changed_ = true;
			stored_paths_.erase(file_path);
		}
	}
}

void PathStorage::ScanForGameServices() {
	vector<filesystem::path> all_game_paths = glparse::parseLauncherPaths();
	AddNewPaths(all_game_paths);
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

void PathStorage::restorePathsAndConfig() {
	if (!filesystem::exists(PATH_STORAGE_FILENAME)) {
		return;
	}

#if _DEBUG
	cout << "Begin restoring toml" << endl;
#endif

	const toml::value main_toml = toml::parse(PATH_STORAGE_FILENAME);

	if (main_toml.contains("name")) {
		if (main_toml.at("name").as_string() != PROGRAM_NAME) {
			return;
		}
	}
	else {
		return;
	}

	restorePaths(main_toml);
	restoreConfig(main_toml);
#if _DEBUG
	cout << "We successfully restored toml" << endl;
#endif
}

void PathStorage::restorePaths(const toml::value& main_toml) {
	if (main_toml.contains("dll_paths")) {
		filesystem::path tmp_path;
		for (const auto& file_path : main_toml.at("dll_paths").as_array()) {
			tmp_path = file_path.as_string();
			if (filesystem::exists(tmp_path / DLSS_DLL_NAME)) {
				stored_paths_.emplace(tmp_path);
			}
		}
	}
	
#if _DEBUG
	cout << "We successfully restored paths. Current number of paths is " << stored_paths_.size() << endl;
#endif
}

void PathStorage::restoreConfig(const toml::value& main_toml) {
	if (main_toml.contains("dll_dir_path")) {
		is_dll_dir_path_changed_ = true;
		dll_dir_path_ = main_toml.at("dll_dir_path").as_string();
	}
#if _DEBUG
	cout << "We successfully restored config" << endl;
#endif
}

void PathStorage::savePathsAndConfig() const {
	if (stored_paths_.empty() && !stored_paths_changed_ && !is_dll_dir_path_changed_) {
		return;
	}

	toml::value main_toml;
	main_toml["name"] = PROGRAM_NAME;

	savePaths(main_toml);
	saveConfig(main_toml);

	fstream dll_path_storage_file(PATH_STORAGE_FILENAME, ios::out);
	if (!dll_path_storage_file.good()) {
		throw runtime_error(PATH_STORAGE_FILENAME + " cannot be opened");
	}

	dll_path_storage_file << toml::format(main_toml);

#if _DEBUG
	cout << "Class was stored successfully" << endl;
#endif
}

void PathStorage::savePaths(toml::value& main_toml) const {
	if (stored_paths_.empty() && !stored_paths_changed_) {
		return;
	}

	toml::value path_storage_toml(toml::array{});
	for (const auto& file_path : stored_paths_) {
		path_storage_toml.emplace_back(file_path.generic_string());
	}

	main_toml["dll_paths"] = path_storage_toml;

#if _DEBUG
	cout << "All paths were stored successfully" << endl;
#endif
}

void PathStorage::saveConfig(toml::value& main_toml) const {
	if (!is_dll_dir_path_changed_) {
		return;
	}
	main_toml["dll_dir_path"] = dll_dir_path_.generic_string();

#if _DEBUG
	cout << "Config was stored successfully" << endl;
#endif
}