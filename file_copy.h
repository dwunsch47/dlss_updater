#pragma once

#include <unordered_map>
#include <filesystem>
#include <string>
#include <tuple>

void fileCopy(const std::unordered_map<std::filesystem::path, std::tuple<bool, std::string>>& paths, const std::filesystem::path dll_path);


