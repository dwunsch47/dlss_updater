#include "file_utilities.h"

#include <Windows.h>
#include <filesystem>
#include <sstream>
#include <string>
#include <tuple>

#include <WinReg.hpp>

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
		return output.str();
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

		return { major_ver, minor_ver, bug_ver, build_num_ver };
	}

	string getPathFromRegistry(const wstring& reg_path, const wstring& reg_value) {
		winreg::RegKey key;
		const auto try_open = key.TryOpen(HKEY_LOCAL_MACHINE, reg_path, KEY_READ);
		if (try_open.Failed()) {
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

	vector<wstring> getSubkeys(const wstring& reg_path) {
		vector<wstring> result;
		winreg::RegKey key;
		const auto try_open = key.TryOpen(HKEY_LOCAL_MACHINE, reg_path, KEY_READ);
		if (try_open.Failed()) {
			return result;
		}
		const auto& enum_try = key.TryEnumSubKeys();
		if (enum_try.IsValid()) {
			result = enum_try.GetValue();
		}
		return result;
	}
} // namespace fileUtil