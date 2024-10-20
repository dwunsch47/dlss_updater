#pragma once

#include <filesystem>
#include <string>

namespace fileUtil {
	std::string GetDLLVersion(const std::filesystem::path& file_path);
}
