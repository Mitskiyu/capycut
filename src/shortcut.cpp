#include <codecvt>
#include <filesystem>
#include <iostream>
#include <locale>
#include <string>

#include <combaseapi.h>
#include <knownfolders.h>
#include <shlobj_core.h>
#include <windows.h>

#include "shortcut.hpp"

std::wstring Shortcut::LocateStartMenuPath()
{
	PWSTR path = nullptr;
	// locate programs folder for user
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Programs, 0, NULL, &path);
	if (!SUCCEEDED(hr) || path == nullptr)
	{
		std::wcerr << L"ERROR: Could not get Start Menu path" << '\n';
		std::string manualPath;
		std::getline(std::cin, manualPath);

		if (!std::filesystem::exists(manualPath))
			return L"";

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
		std::wstring wmanual = conv.from_bytes(manualPath);

		return wmanual;
	}

	std::wstring startMenuPath(path);
	CoTaskMemFree(path);
	return startMenuPath;
}
