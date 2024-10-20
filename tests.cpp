#include "tests.h"
#include "file_utilities.h"

#include <vector>
#include <filesystem>
#include <tuple>
#include <fstream>
#include <iostream>
#include <cassert>

using namespace std;

void TestPathStorage() {
	const filesystem::path root_working_dir = filesystem::current_path();
	filesystem::path current_working_dir = root_working_dir / "path_storage_test";
	filesystem::create_directory(current_working_dir);
	cout << "Current working directory is : " << current_working_dir << endl;

	{
		filesystem::remove(current_working_dir / PATH_STORAGE_FILENAME);
		cout << "FIRST TEST" << endl;

		const filesystem::path test_dir_path = current_working_dir / "first_dir";
		filesystem::create_directory(test_dir_path);
		fstream first_file(test_dir_path / DLSS_DLL_NAME, ios::out);
		vector<filesystem::path> test_paths = { test_dir_path };

		PathStorage test_storage(current_working_dir);
		test_storage.AddNewPaths(test_paths);
		assert(test_storage.GetStoredPaths().size() == test_paths.size());
	}

	{
		filesystem::remove(current_working_dir / PATH_STORAGE_FILENAME);
		cout << "SECOND TEST" << endl;

		const filesystem::path test_dir_path = current_working_dir / "second_dir";
		filesystem::create_directory(test_dir_path);
		fstream first_file(test_dir_path / DLSS_DLL_NAME, ios::out);
		vector<filesystem::path> test_paths = { test_dir_path, test_dir_path / DLSS_DLL_NAME };

		PathStorage test_storage(current_working_dir);
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
}