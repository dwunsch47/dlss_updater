#pragma once

#include <Windows.h>
#include <filesystem>
#include <utility>
#include <string>

//int GetDLLVersion(const std::filesystem::path& file_path, std::shared_ptr<char[]> version_info);
std::string GetDLLVersion(const std::filesystem::path& file_path);

