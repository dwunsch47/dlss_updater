#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <tuple>
#include <unordered_set>

class PathStorage {
public:
	explicit PathStorage();
	explicit PathStorage(std::filesystem::path path_to_dll);

	~PathStorage();

	void AddNewPaths(std::vector<std::filesystem::path> new_paths);
	void RemovePaths(const std::vector<std::filesystem::path>& paths_for_removal);

	const std::unordered_set<std::filesystem::path>& GetStoredPaths() const;
	std::filesystem::path GetDLLPath() const;
	
private:
	std::unordered_set<std::filesystem::path> stored_paths_;
	bool new_paths_added_ = false;
	bool is_config_properly_formatted_ = false;
	std::filesystem::path dll_dir_path_ = std::filesystem::current_path();

	void restoreSavedFilePaths();
	void savePathsAndConfig() const;
	void savePaths() const;
	void saveConfig() const;
};