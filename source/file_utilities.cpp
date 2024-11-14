#include "file_utilities.h"

#include <Windows.h>
#include <filesystem>
#include <sstream>
#include <string>
#include <tuple>

using namespace std;

namespace fileUtil {
	string getDLLVersion(filesystem::path file_path) {
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

	tuple<int, int, int, int> formatDLLVersion(const string file_version) {
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
}