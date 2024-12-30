#pragma once

#include "game_launcher_parser.h"

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <tuple>
#include <mutex>
#include <unordered_set>
#include <unordered_map>

#include <toml.hpp>

class PathStorage {
public:
	explicit PathStorage();

	~PathStorage();

	void AddDllPath(const std::vector<std::filesystem::path>& dll_path);
	void RestoreDllPath();

	void AddNewPaths(const std::vector<std::filesystem::path>& new_paths);
	void RemovePaths(const std::vector<std::filesystem::path>& paths_for_removal);

	void ScanForGameServices();

	const std::unordered_set<std::filesystem::path>& GetStoredPaths() const;
	std::filesystem::path GetDLLPath() const;

	void ShowStoredDllsVersions() const;
	
private:
	std::unordered_set<std::filesystem::path> stored_paths_;
	std::mutex mutex_stored_paths_;
	bool stored_paths_changed_ = false;
	bool is_dll_dir_path_changed_ = false;
	std::filesystem::path dll_dir_path_ = std::filesystem::current_path();

	void restorePathsAndConfig();
	void restorePaths(const toml::value& main_toml);
	void restoreConfig(const toml::value& main_toml);

	void savePathsAndConfig() const;
	void savePaths(toml::value& main_toml) const;
	void saveConfig(toml::value& main_toml) const;

	void checkDirectoryPath(const std::filesystem::path& dir_path);
};