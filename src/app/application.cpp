#include "application.h"
#include "path_storage.h"
#include "file_utilities.h"
#include "game_launcher_parser.h"
#include "file_copy.h"

#include <memory>
#include <iostream>

#include <toml.hpp>

using namespace std;

Application::Application(int& argc, char** argv) {
	if (argc < 2) {
		return;
	}
	restoreApplication();
	parseCmdArgs(argc, argv);
}

Application::~Application() {
	saveApplication();
}

void Application::parseCmdArgs(int& argc, char** argv) {
	// TODO: switch to immediate mode parser, or handle it more gracefully
	const string mode = argv[1];
	vector<filesystem::path> args;
	if (argc > 2) {
		for (int i = 2; i < argc; ++i) {
			args.emplace_back(argv[i]);
		}
	}

	if (mode == "scan") {
		scanForGameServices();
	}
	else if (mode == "add") {
		ps_ptr_->AddNewPaths(move(args));
	}
	else if (mode == "dll_path") {
		settings_storage_.dll_path = move(args[0]);
		are_settings_changed_ = true;
	}
	else if (mode == "restore_dll_path") {
		if (settings_storage_.dll_path != filesystem::current_path()) {
			settings_storage_.dll_path = filesystem::current_path();
			are_settings_changed_ = true;
		}
	}
	else if (mode == "update") {
		// TODO: add options to --force-update even is installed version is newer
		fileCopy(ps_ptr_->GetStoredPaths(), getDllPath());
	}
	// TODO: add option to --force-keep old, obsolete paths
	else if (mode == "remove") {
		ps_ptr_->RemovePaths(args);
	}
	else if (mode == "show") {
		showStoredDllVers();
	}
}

void Application::showStoredDllVers() const {
	if (filesystem::exists(settings_storage_.dll_path / DLSS_DLL_NAME)) {
		cout << "Latest available DLSS version is: " << fileUtil::getDLLVersion(settings_storage_.dll_path / DLSS_DLL_NAME) << '\n';
	}
	else {
		cout << "No available DLSS dll. Add DLSS dll to current program or add path to DLSS dll's folder" << '\n';
	}

	const auto& stored_paths = ps_ptr_->GetStoredPaths();
	if (stored_paths.empty()) {
		cout << "No game folders with DLSS were added. Use \"scan\" or \"add\" to add folders" << '\n';
		return;
	}

	filesystem::path full_dll_path;
	for (const auto& file_path : stored_paths) {
		full_dll_path = file_path / DLSS_DLL_NAME;
		if (filesystem::exists(full_dll_path)) {
			cout << '"' << full_dll_path.generic_string() << "\", version: " << fileUtil::getDLLVersion(full_dll_path) << '\n';
		}
	}
}

filesystem::path Application::getDllPath() const {
	return settings_storage_.dll_path;
}

void Application::scanForGameServices() {
#if _DEBUG
	cout << "Began scan" << endl;
#endif
	ps_ptr_->AddNewPaths(glparse::parseLauncherPaths());
}

void Application::restoreApplication() {
#if _DEBUG
	cout << "Restore: application" << endl;
#endif

	vector<filesystem::path> parsed_paths;

	if (filesystem::exists(APP_CONFIG_FILENAME)) {
#if _DEBUG
		cout << "Restore: toml" << endl;
#endif
		const toml::value main_toml = toml::parse(APP_CONFIG_FILENAME);

		if (main_toml.contains("name")) {
			if (main_toml.at("name").as_string() != PROGRAM_NAME) {
				return;
			}
		}
		else {
			return;
		}

		parsed_paths = restorePathStorage(main_toml);
		restoreSettings(main_toml);

#if _DEBUG
		cout << "Restore successful: toml" << endl;
#endif
	}
		ps_ptr_ = make_unique<PathStorage>(move(parsed_paths));


#if _DEBUG
	cout << "Restore successful: application" << endl;
#endif
}

vector<filesystem::path> Application::restorePathStorage(const toml::value& main_toml) {
	vector<filesystem::path> result;
	if (main_toml.contains("dll_paths"s)) {
		result.reserve(main_toml.at("dll_paths"s).size());
		for (const auto& path : main_toml.at("dll_paths"s).as_array()) {
			result.emplace_back(path.as_string());
		}
	}
	return result;
}

void Application::restoreSettings(const toml::value& main_toml) {
	if (main_toml.is_empty()) {
		return;
	}

	if (main_toml.contains("dll_dir_path"s)) {
		if (filesystem::exists(main_toml.at("dll_dir_path"s).as_string())) { // if we oneline this check it can throw, so we dont
			settings_storage_.dll_path = main_toml.at("dll_dir_path"s).as_string();
		}
	}
}

void Application::saveApplication() const {
	if (!ps_ptr_->ArePathsChanged() && !are_settings_changed_) {
		return;
	}
#if _DEBUG
	cout << "Save: application" << endl;
#endif

	toml::value main_toml;
	main_toml["name"] = PROGRAM_NAME;

	savePathStorage(main_toml);
	saveSettings(main_toml);

	fstream dll_path_storage_file(APP_CONFIG_FILENAME, ios::out);
	if (!dll_path_storage_file.good()) {
		throw runtime_error(APP_CONFIG_FILENAME + " cannot be opened");
	}

	dll_path_storage_file << toml::format(main_toml);

#if _DEBUG
	cout << "Saved successfully: application" << endl;
#endif
}

void Application::savePathStorage(toml::value& main_toml) const {
	const auto& stored_paths_ref = ps_ptr_->GetStoredPaths();

	toml::value path_storage_toml(toml::array{});
	for (const auto& file_path : stored_paths_ref) {
		path_storage_toml.emplace_back(file_path.generic_string());
	}

	main_toml["dll_paths"] = path_storage_toml;

#if _DEBUG
	cout << "Saved successfully: paths" << endl;
#endif
}

void Application::saveSettings(toml::value& main_toml) const {
	if (settings_storage_.dll_path != filesystem::current_path()) {
		main_toml["dll_dir_path"] = settings_storage_.dll_path.generic_string();
	}

#if _DEBUG
	cout << "Saved successfully: settings" << endl;
#endif
}