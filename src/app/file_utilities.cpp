#include "file_utilities.h"

#include <Windows.h>
#include <filesystem>
#include <sstream>
#include <string>
#include <tuple>

#include <WinReg.hpp>
#include <json.hpp>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <vdf_parser.hpp>
#pragma warning(pop)

using namespace std;

namespace fileUtil {
	string getDLLVersion(const filesystem::path& file_path) {
		DWORD dwHandle, dwLen = GetFileVersionInfoSizeA(file_path.string().c_str(), &dwHandle);
		if (dwLen == 0) {
			return "error";
		}

		unique_ptr<char> buf(new char[dwLen]);
		if (!GetFileVersionInfoA(file_path.string().c_str(), dwHandle, dwLen, buf.get())) {
			return "error";
		}

		VS_FIXEDFILEINFO* fileinfo = NULL;
		UINT len = 0;
		if (!VerQueryValueA(buf.get(), "\\", (LPVOID*)&fileinfo, &len)) {
			return "error";
		}

		ostringstream output;
		output << HIWORD(fileinfo->dwFileVersionMS) << '.' << LOWORD(fileinfo->dwFileVersionMS) << '.' << HIWORD(fileinfo->dwFileVersionLS) << '.' << LOWORD(fileinfo->dwFileVersionLS);
		return move(output.str());
	}

	tuple<int, int, int, int> formatDLLVersion(const string& file_version) {
		int major_ver = 0, minor_ver = 0, bug_ver = 0, build_num_ver = 0;
		if (file_version == "error") {
			return { 0, 0, 0, 0 };
		}
		
		size_t pos = 0, prev_pos = 0;
		pos = file_version.find('.', prev_pos);
		major_ver = stoi(file_version.substr(prev_pos, pos++ - prev_pos));
		prev_pos = pos;
		pos = file_version.find('.', prev_pos);
		minor_ver = stoi(file_version.substr(prev_pos, pos++ - prev_pos));
		prev_pos = pos;
		pos = file_version.find('.', prev_pos);
		bug_ver = stoi(file_version.substr(prev_pos, pos++ - prev_pos));
		prev_pos = pos;
		pos = file_version.find('.', prev_pos);
		build_num_ver = stoi(file_version.substr(prev_pos, pos++ - prev_pos));

		tuple<int, int, int, int> result = { major_ver, minor_ver, bug_ver, build_num_ver };

		return result;
	}

	string getPathFromRegistry(const wstring& reg_path, const wstring& reg_value) {
		winreg::RegKey key;
		const auto open_result = key.TryOpen(HKEY_LOCAL_MACHINE, reg_path);
		if (!open_result) {
			return "";
		}
		if (key.ContainsValue(reg_value)) {
			const auto val_open_result = key.TryGetStringValue(reg_value);
			if (val_open_result.IsValid()) {
				const wstring regVal = val_open_result.GetValue();
				int str_size = WideCharToMultiByte(CP_UTF8, 0, regVal.data(), (int)regVal.size(), NULL, 0, NULL, NULL);
				string str_result(str_size, 0);
				WideCharToMultiByte(CP_UTF8, 0, regVal.data(), (int)regVal.size(), str_result.data(), str_size, NULL, NULL);
				return str_result;
			}
		}
		return "";
	}

	namespace parsers {
		vector<filesystem::path> parseVdf(const filesystem::path& file_path) {
			const filesystem::path vdf_path = file_path / STEAM_LIBRARYFOLDERS_PATH;
			if (!filesystem::exists(vdf_path)) {
				return {};
			}
			std::ifstream vdf_file(vdf_path);
			auto root = tyti::vdf::read(vdf_file);
			vdf_file.close();
			if (root.name == "libraryfolders") {
				vector<filesystem::path> result;
				result.reserve(root.childs.size());
				filesystem::path tmp_path;
				for (const auto& [field_name, ptr] : root.childs) {
					tmp_path = ptr->attribs["path"];
					result.push_back(tmp_path / STEAM_GAMES_PATH_POSTFIX);
				}
				return result;
			}
			else {
				return {};
			}
		}

		vector<filesystem::path> parseEgsManifests(const filesystem::path& file_path) {
			vector<filesystem::path> result;
			for (const auto& parse_path : filesystem::directory_iterator(file_path / EGS_MANIFESTS_PATH)) {
				if (!filesystem::is_regular_file(parse_path)) {
					continue;
				}
				ifstream item_file(parse_path.path());
				nlohmann::json parsed_item = nlohmann::json::parse(item_file);
				item_file.close();
				if (parsed_item.find("InstallLocation") != parsed_item.end()) {
					result.push_back(parsed_item.at("InstallLocation"));
				}
			}
			return result;
		}
	}
}