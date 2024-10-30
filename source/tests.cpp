#include "tests.h"
#include "file_utilities.h"

#include <vector>
#include <filesystem>
#include <tuple>
#include <fstream>
#include <iostream>
#include <cassert>
#include <chrono>

using namespace std;

void TestPathStorage() {
	if (filesystem::exists(PATH_STORAGE_FILENAME)) {
		filesystem::rename(PATH_STORAGE_FILENAME, (PATH_STORAGE_FILENAME + ".orig"));
	}
	if (filesystem::exists(DLSS_DLL_NAME)) {
		filesystem::rename(DLSS_DLL_NAME, (DLSS_DLL_NAME + ".orig"));
	}
	const filesystem::path root_dir = filesystem::current_path();
	const filesystem::path current_working_dir = root_dir / "test_dir";
	filesystem::create_directory(current_working_dir);

	{
		filesystem::remove(PATH_STORAGE_FILENAME);
		cout << "FIRST TEST" << endl;

		filesystem::path test_dir = current_working_dir / "testdir1";
		filesystem::create_directory(test_dir);
		fstream first_file(test_dir / DLSS_DLL_NAME, ios::out);
		vector<filesystem::path> test_paths = { test_dir };

		PathStorage test_storage(test_dir);
		test_storage.AddNewPaths(test_paths);
		assert(test_storage.GetStoredPaths().size() == test_paths.size());
	}

	{
		filesystem::remove(PATH_STORAGE_FILENAME);
		cout << "SECOND TEST" << endl;

		const filesystem::path test_dir = current_working_dir / "testdir2";
		filesystem::create_directory(test_dir);
		fstream first_file(test_dir / DLSS_DLL_NAME, ios::out);
		vector<filesystem::path> test_paths = { test_dir, test_dir / DLSS_DLL_NAME };

		PathStorage test_storage(test_dir);
		test_storage.AddNewPaths(test_paths);
		assert(test_storage.GetStoredPaths().size() == 1);
	}


	{
		cout << "THIRD TEST formatDLLVersion" << endl;
		tuple<int, int, int, int> test1 = fileUtil::formatDLLVersion("3.7.20.0");
		tuple<int, int, int, int> test1_result = { 3, 7, 20, 0 };
		cout << "RESULT: " << get<0>(test1) << '.' << get<1>(test1) << '.' << get<2>(test1) << '.' << get<3>(test1) << endl;
		assert(test1_result == test1);
	}

	{
		cout << "THIRD TEST formatDLLVersion" << endl;
		tuple<int, int, int, int> test1 = fileUtil::formatDLLVersion("40.228.0.120");
		tuple<int, int, int, int> test1_result = { 40, 228, 0, 120 };
		cout << "RESULT: " << get<0>(test1) << '.' << get<1>(test1) << '.' << get<2>(test1) << '.' << get<3>(test1) << endl;
		assert(test1_result == test1);
	}
	
	filesystem::remove_all(current_working_dir);

	if (filesystem::exists(PATH_STORAGE_FILENAME + ".orig")) {
		filesystem::rename((PATH_STORAGE_FILENAME + ".orig"), PATH_STORAGE_FILENAME);
	}
	if (filesystem::exists(DLSS_DLL_NAME + ".orig")) {
		filesystem::rename(DLSS_DLL_NAME + ".orig", DLSS_DLL_NAME);
	}
}