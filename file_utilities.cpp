#include "file_utilities.h"

#include <Windows.h>
#include <filesystem>
#include <utility>
#include <cstdio>
#include <sstream>

using namespace std;

string GetDLLVersion(const filesystem::path& file_path) {
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
	if (!VerQueryValueA(buf.get(), "\\", (LPVOID *)&fileinfo, &len)) {
		return "error";
	}

	ostringstream output;
	output << HIWORD(fileinfo->dwFileVersionMS) << '.' << LOWORD(fileinfo->dwFileVersionMS) << '.' << HIWORD(fileinfo->dwFileVersionLS) << '.' << LOWORD(fileinfo->dwFileVersionLS);
	//string output_str = output.str();
	return output.str();
}