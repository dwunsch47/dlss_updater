#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <tuple>
#include <mutex>
#include <unordered_set>

class PathStorage {
public:
	PathStorage() = default;
	PathStorage(std::vector<std::filesystem::path> paths_to_restore);

	void AddNewPaths(std::vector<std::filesystem::path> new_paths);
	void RemovePaths(const std::vector<std::filesystem::path>& paths_for_removal);

	const std::unordered_set<std::filesystem::path>& GetStoredPaths() const;
	bool ArePathsChanged() const;
	
private:
	std::unordered_set<std::filesystem::path> stored_paths_;
	std::mutex mutex_stored_paths_;
	bool are_stored_paths_changed_ = false;

	void restorePaths(std::vector<std::filesystem::path> paths_to_restore);

	void checkDirectoryPath(std::filesystem::directory_entry dir_path);
};