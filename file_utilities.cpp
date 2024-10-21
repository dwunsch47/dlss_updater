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
		return output.str();
	}

	tuple<int, int, int, int> formatDLLVersion(const string& file_version) {

		if (file_version == "error") {
			return { 0, 0, 0, 0 };
		}
		vector<int> major_minor_bug_build;
		major_minor_bug_build.reserve(4);
		size_t pos = 0, prev_pos = 0;
		for (int i = 0; i < 4; ++i) {
			pos = file_version.find('.', prev_pos);
			major_minor_bug_build.push_back(stoi(file_version.substr(prev_pos, pos++ - prev_pos)));
			prev_pos = pos;
		}
		
		return { major_minor_bug_build[0], major_minor_bug_build[1], major_minor_bug_build[2], major_minor_bug_build[3] };
	}
}