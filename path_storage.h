#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <string>

const std::string PATH_STORAGE_FILENAME = "locations.txt";

class PathStorage {
public:
	explicit PathStorage();

	~PathStorage();

	void AddNewPaths(const std::vector<std::filesystem::path>& new_paths);
	const std::map<std::filesystem::path, bool>& GetStoredPaths() const;
private:
	std::map<std::filesystem::path, bool> stored_path_to_recency;
	bool is_changed = false;

	void restoreSavedFilePaths();
	void saveFilePaths() const;
};