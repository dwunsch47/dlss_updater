#pragma once

#include <unordered_set>
#include <filesystem>
#include <string>
#include <tuple>

void fileCopy(const std::unordered_set<std::filesystem::path>& paths, const std::filesystem::path dll_path);


