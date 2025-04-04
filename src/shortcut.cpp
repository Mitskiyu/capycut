#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <locale>
#include <regex>
#include <string>

#include <combaseapi.h>
#include <knownfolders.h>
#include <shlobj_core.h>
#include <windows.h>

#include "shortcut.hpp"
#include "steam.hpp"

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

std::wstring Shortcut::CreateSubDir(const std::wstring& startMenuPath)
{
	std::wstring subDir = L"Capycut";
	std::wstring fullPath = startMenuPath + L"\\" + subDir;

	std::filesystem::create_directories(fullPath);

	return fullPath;
}

void Shortcut::CreateShortcut(const Steam::AppData& data, const std::wstring& subDirPath)
{
	// invalid filename characters
	std::regex invalid(R"([\\/*?:"<>;|])");

	// sanitize the filename, replace with _
	std::string name = std::regex_replace(data.name, invalid, "_");

	// convert to wstring for filename
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	std::wstring wname = conv.from_bytes(name);

	std::wstring wfilePath = subDirPath + L"\\" + wname + L".url";

	std::string filePath = conv.to_bytes(wfilePath);
	std::ofstream ofs(filePath);
	if (!ofs)
	{
		std::wcerr << L"ERROR: Could not create shortcut for file: " << wfilePath << '\n';
		return;
	}

	// write url file
	ofs << "[InternetShortcut]\n";
	ofs << "URL=steam://rungameid/" << data.appId << '\n';

	ofs.close();
}
