#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <locale>
#include <regex>
#include <string>
#include <vector>

#include <windows.h>
#include <winnls.h>
#include <winreg.h>

#include "steam.hpp"

std::wstring Steam::LocateSteamPath()
{
	// ask user for path on error
	auto LocateSteamPathManual = []()
		{
			std::wcout << L"Manually input the Steam installation path: " << L'\n';
			std::wstring steamPath;
			std::getline(std::wcin, steamPath);

			if (!std::filesystem::exists(steamPath))
			{
				return std::wstring{};
			}

			return steamPath;
		};

	HKEY hKey;
	// steam registry path
	const char* subKey = "SOFTWARE\\WOW6432Node\\Valve\\Steam";

	// opens registry key at subkey, writes handle to hkey
	LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hKey);
	if (result != ERROR_SUCCESS)
	{
		std::wcerr << L"ERROR: Could not open registry key: " << result << L'\n';
		return LocateSteamPathManual();
	}

	char steamPath[512]{};
	DWORD bufferSize = sizeof(steamPath);

	// retrieve data for registry value InstallPath, writes to buffer
	result = RegQueryValueExA(hKey, "InstallPath", nullptr, nullptr, reinterpret_cast<LPBYTE>(steamPath), &bufferSize);
	RegCloseKey(hKey);
	if (result != ERROR_SUCCESS)
	{
		std::wcerr << L"ERROR: Could not read install path: " << result << L'\n';
		return LocateSteamPathManual();
	}

	// conv char arr to wstring
	std::wstring wsteamPath;
	int convSize = MultiByteToWideChar(CP_ACP, 0, steamPath, -1, NULL, 0);
	// account for null terminator
	wsteamPath.resize(convSize - 1);
	MultiByteToWideChar(CP_ACP, 0, steamPath, -1, &wsteamPath[0], convSize);
	return wsteamPath;
}

std::wstring Steam::LocateLibraryVDF(const std::wstring& steamPath)
{
	// construct libraryfolders.vdf path
	std::filesystem::path vdfPath = std::filesystem::path(steamPath) / "steamapps" / "libraryfolders.vdf";
	if (!std::filesystem::exists(vdfPath))
	{
		std::wcerr << L"ERROR: Could not find libraryfolders.vdf" << L'\n';
		std::wcout << L"Manually input the path to libraryfolders.vdf: " << L'\n';
		std::wstring manualPath;
		std::getline(std::wcin, manualPath);

		vdfPath = std::filesystem::path(manualPath);
		if (!std::filesystem::exists(vdfPath))
		{
			return std::wstring{};
		}
	}

	return vdfPath.wstring();
}

std::vector<std::wstring> Steam::LocateLibraryPaths(const std::wstring& vdfPath) {
	std::vector<std::wstring> libraryPaths;
	std::wifstream file(vdfPath);
	if (!file.is_open())
	{
		std::wcerr << L"ERROR: Could not open: " << vdfPath << L'\n';
		return libraryPaths;
	}

	std::wregex path(LR"x("path"\s+"([^"]+)")x");
	std::wstring line;
	std::wsmatch match;

	while (std::getline(file, line))
	{
		if (std::regex_search(line, match, path))
		{
			if (match.size() >= 2)
				libraryPaths.push_back(match[1].str());
		}
	}

	return libraryPaths;
}

std::vector<std::wstring> Steam::LocateAppManifestPaths(const std::wstring& libraryPath)
{
	std::vector<std::wstring> appManifestPaths;

	// construct steamapps path
	std::filesystem::path steamappsPath = std::filesystem::path(libraryPath) / "steamapps";
	if (!std::filesystem::exists(steamappsPath))
	{
		std::wcerr << L"ERROR: Could not find steamapps dir in library: " << libraryPath << L'\n';
		std::wcout << L"Manually input the 'steamapps' dir path: " << L'\n';
		std::wstring manualPath;
		std::getline(std::wcin, manualPath);

		steamappsPath = std::filesystem::path(manualPath);
		if (!std::filesystem::exists(steamappsPath))
			return appManifestPaths;
	}

	// iterate over entries in steamapps
	for (const auto& entry : std::filesystem::directory_iterator(steamappsPath))
	{
		if (entry.is_regular_file())
		{
			std::filesystem::path filePath = entry.path();
			std::wstring filename = filePath.filename().wstring();

			if (filename.find(L"appmanifest_") == 0 && filePath.extension() == L".acf")
				appManifestPaths.push_back(filePath.wstring());
		}
	}

	return appManifestPaths;
}

Steam::AppData Steam::ParseAppManifest(const std::wstring& appManifestPath)
{
	AppData appData;

	// utf8 encoding
	std::wifstream file(appManifestPath);
	file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
	if (!file.is_open())
	{
		std::wcerr << L"ERROR: Could not open: " << appManifestPath << L'\n';
		return appData;
	}

	std::wregex appId(LR"x("appid"\s+"(\d+)")x");
	std::wregex name(LR"x("name"\s+"([^"]+)")x");
	std::wregex installDir(LR"x("installdir"\s+"([^"]+)")x");
	std::wstring line;
	std::wsmatch match;

	while (std::getline(file, line))
	{
		if (std::regex_search(line, match, appId) && match.size() >= 2)
			appData.appId = std::stoi(match[1].str());
		if (std::regex_search(line, match, name) && match.size() >= 2)
			appData.name = match[1].str();
		if (std::regex_search(line, match, installDir) && match.size() >= 2)
			appData.installDir = match[1].str();
	}

	return appData;
}