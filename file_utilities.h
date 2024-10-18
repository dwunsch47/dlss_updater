#pragma once

#include <filesystem>
#include <string>

//int GetDLLVersion(const std::filesystem::path& file_path, std::shared_ptr<char[]> version_info);
std::string GetDLLVersion(const std::filesystem::path& file_path);

