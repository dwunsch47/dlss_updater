#pragma once


#include <filesystem>
#include <string>
#include <tuple>
#include <map>

void fileCopy(const std::map<std::filesystem::path, std::tuple<bool, std::string>>& paths, const std::filesystem::path dll_path);


