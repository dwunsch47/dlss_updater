#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <string>
#include <tuple>

const std::string PATH_STORAGE_FILENAME = "dll_locations.txt";
const std::string DLSS_DLL_NAME = "nvngx_dlss.dll";
const std::string PROGRAM_NAME = "DLSS Updater";

class PathStorage {
public:
	explicit PathStorage();
	explicit PathStorage(std::filesystem::path path_to_config_dir);

	~PathStorage();

	void AddNewPaths(const std::vector<std::filesystem::path>& new_paths);
	const std::map<std::filesystem::path, std::tuple<bool, std::string>>& GetStoredPaths() const;
	
private:
	std::map <std::filesystem::path, std::tuple<bool, std::string>> stored_path_to_recency_version_;
	bool is_changed_ = false;
	bool is_config_properly_formatted_ = false;
	const std::filesystem::path config_dir_location_ = std::filesystem::current_path();

	void restoreSavedFilePaths();
	void saveFilePaths() const;
};