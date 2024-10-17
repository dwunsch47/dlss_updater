#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <string>

const std::string PATH_STORAGE_FILENAME = "locations.txt";
const std::string DLSS_DLL_NAME = "nvngx_dlss.dll";

class PathStorage {
public:
	explicit PathStorage();
	explicit PathStorage(std::filesystem::path path_to_config_dir);

	~PathStorage();

	void AddNewPaths(const std::vector<std::filesystem::path>& new_paths);
	const std::map<std::filesystem::path, bool>& GetStoredPaths() const;
private:
	std::map<std::filesystem::path, bool> stored_path_to_recency_;
	bool is_changed = false;
	const std::filesystem::path config_dir_location_ = std::filesystem::current_path();

	void restoreSavedFilePaths();
	void saveFilePaths() const;
};