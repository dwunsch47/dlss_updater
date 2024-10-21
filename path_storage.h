#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <string>
#include <tuple>

class PathStorage {
public:
	explicit PathStorage();
	explicit PathStorage(std::filesystem::path path_to_dll);

	~PathStorage();

	void AddNewPaths(std::vector<std::filesystem::path> new_paths);
	void RemovePaths(std::vector<std::filesystem::path> paths_for_removal);

	const std::map<std::filesystem::path, std::tuple<bool, std::string>>& GetStoredPaths() const;
	const std::filesystem::path& GetDLLPath() const;
	
private:
	std::map <std::filesystem::path, std::tuple<bool, std::string>> stored_path_to_recency_version_;
	bool new_paths_added_ = false;
	bool is_config_properly_formatted_ = false;
	bool are_any_lines_for_deletion_ = false;
	const std::filesystem::path config_dir_location_ = std::filesystem::current_path();
	std::filesystem::path dll_location_ = config_dir_location_;

	void restoreSavedFilePaths();
	void saveFilePaths() const;
};