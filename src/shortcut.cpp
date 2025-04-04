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
		std::wcerr << L"ERROR: Could not get Start Menu path" << L'\n';
		std::wstring manualPath;
		std::getline(std::wcin, manualPath);

		if (!std::filesystem::exists(manualPath))
			return L"";

		// std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
		// std::wstring wmanual = conv.from_bytes(manualPath);

		return manualPath;
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
	std::wregex invalid(LR"([\\/*?:"<>;|])");

	// sanitize the filename, replace with _
	std::wstring name = std::regex_replace(data.name, invalid, L"_");

	// convert to wstring for filename
	// std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	// std::wstring wname = conv.from_bytes(name);

	std::wstring filePath = subDirPath + L"\\" + name + L".url";

	// std::string filePath = conv.to_bytes(wfilePath);

	// utf8 encoding
	std::wofstream ofs(filePath);
	ofs.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
	if (!ofs)
	{
		std::wcerr << L"ERROR: Could not create shortcut for file: " << filePath << L'\n';
		return;
	}

	// write url file
	ofs << L"[InternetShortcut]\n";
	ofs << L"URL=steam://rungameid/" << data.appId << L'\n';

	ofs.close();
}
