#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <tuple>
#include <mutex>
#include <unordered_set>
#include <unordered_map>

struct GameLauncherData {
	std::wstring reg_path;
	std::wstring reg_value;
};

class PathStorage {
public:
	explicit PathStorage();
	explicit PathStorage(std::filesystem::path path_to_dll);

	~PathStorage();

	void AddNewPaths(const std::vector<std::filesystem::path>& new_paths);
	void RemovePaths(const std::vector<std::filesystem::path>& paths_for_removal);

	void ScanForGameServices();

	const std::unordered_set<std::filesystem::path>& GetStoredPaths() const;
	std::filesystem::path GetDLLPath() const;

	void ShowStoredDllsVersions() const;
	
private:
	std::unordered_set<std::filesystem::path> stored_paths_;
	std::mutex mutex_stored_paths_;
	bool new_paths_added_ = false;
	bool is_config_properly_formatted_ = false;
	std::filesystem::path dll_dir_path_ = std::filesystem::current_path();

	const std::unordered_map<std::string, GameLauncherData> launcher_name_to_data_ = {
		{ "steam", { L"SOFTWARE\\Wow6432Node\\Valve\\Steam", L"InstallPath" } },
		{ "egs", { L"SOFTWARE\\WOW6432Node\\Epic Games\\EpicGamesLauncher", L"AppDataPath" }}
	};

	void restoreSavedFilePaths();
	void savePathsAndConfig() const;
	void savePaths() const;
	void saveConfig() const;

	void checkDirectoryPath(const std::filesystem::path& dir_path);

	void scanSteamFolder();
};