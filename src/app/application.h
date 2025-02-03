#pragma once
#include "path_storage/path_storage.h"

#include <memory>
#include <string>

#include <toml.hpp>

const std::string APP_CONFIG_FILENAME = "dll_locations.toml";
const std::string PROGRAM_NAME = "DLSS Updater";

struct Settings {
	std::filesystem::path dll_path = std::filesystem::current_path();
};

class Application {
public:
	Application(int& argc, char** argv);
	~Application();

private:
	std::unique_ptr<PathStorage> ps_ptr_;
	Settings settings_storage_;
	bool are_settings_changed_ = false;

	void parseCmdArgs(int& argc, char** argv);
	void scanForGameServices();
	void showStoredDllVers() const;

	std::filesystem::path getDllPath() const;

	void restoreApplication();
	std::vector<std::filesystem::path> restorePathStorage(const toml::value& main_toml);
	void restoreSettings(const toml::value& main_toml);

	void saveApplication() const;
	void savePathStorage(toml::value& main_toml) const;
	void saveSettings(toml::value& main_toml) const;
};