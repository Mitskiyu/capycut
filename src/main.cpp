#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <windows.h>

#include "shortcut.hpp"
#include "steam.hpp"

int main()
{
	if (!SetConsoleOutputCP(CP_UTF8))
		std::wcerr << L"WARNING: Could not set console output to UTF-8" << std::endl;

	Steam steam;

	std::wstring steamPath = steam.getSteamPath();
	if (steamPath.empty())
	{
		std::wcerr << L"ERROR: Steam installation path could not be determined" << std::endl;
		return 1;
	}
	std::wcout << L"Steam installation path: " << steamPath << std::endl;

	std::wstring libraryVDF = steam.getLibraryVDF();
	if (libraryVDF.empty())
	{
		std::wcerr << L"ERROR: libraryfolders.vdf could not be located" << std::endl;
		return 1;
	}

	std::vector<std::wstring> libraries = steam.getLibraryPaths();
	std::wcout << L"\nFound " << libraries.size() << L" library folder(s)" << std::endl;

	std::vector<std::wstring> manifests = steam.getAppManifestPaths();
	std::wcout << L"Found " << manifests.size() << L" manifest file(s)" << std::endl;

	std::vector<Steam::AppData> apps = steam.getAppsData();
	if (apps.empty())
	{
		std::wcout << L"No installed games found, aborting..." << std::endl;
		return 0;
	}
	else
	{
		std::wcout << L"Found " << apps.size() << L" game(s)" << std::endl;
	}

	Shortcut shortcut;

	std::wstring startMenuPath = shortcut.getStartMenuPath();
	if (startMenuPath.empty())
	{
		std::wcerr << L"ERROR: Start Menu path could not be determined" << std::endl;
		return 1;
	}
	std::wcout << L"\nStart Menu path: " << startMenuPath << std::endl;

	std::wstring subDir = shortcut.CreateSubDir(startMenuPath);
	if (subDir.empty() || !std::filesystem::exists(subDir))
	{
		std::wcerr << L"ERROR: Could not create sub-directory for shortcuts" << std::endl;
		return 1;
	}
	std::wcout << L"Shortcuts will be created in: " << subDir << std::endl;

	std::wstring steamIconPath = steamPath + L"\\steam.exe";
	if (!std::filesystem::exists(steamIconPath))
	{
		std::wcerr << L"WARNING: steam.exe not found at " << steamIconPath;
		std::wcerr << L"Shortcuts may not have an icon" << std::endl;
	}

	int shortcutCount = 0;
	for (const auto& app : apps)
	{
		if (app.installDir.empty())
		{
			std::wcerr << L"Skipping " << app.name << L" because install directory is empty" << std::endl;
			continue;
		}

		shortcut.CreateShortcut(app, subDir, steamIconPath);
		shortcutCount++;
	}

	std::wcout << L"\nCreated " << shortcutCount << L" shortcut(s) successfully" << std::endl;

	std::wcout << L"\nPress any key to exit...";
	std::wcin.get();
	return 0;
}
