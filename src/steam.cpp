#include <iostream>
#include <string>
#include <filesystem>

#include <windows.h>

#include "steam.hpp"

std::string Steam::LocateSteamPath()
{
	// ask user for path on error
	auto LocateSteamPathManual = []()
		{
			std::cout << "Manually input the Steam installation path: " << '\n';
			std::string steamPath;
			std::getline(std::cin, steamPath);

			if (!std::filesystem::exists(steamPath))
			{
				return std::string("");
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
		std::cerr << "ERROR: Could not open registry key: " << result << '\n';
		return LocateSteamPathManual();
	}

	char steamPath[512]{};
	DWORD bufferSize = sizeof(steamPath);

	// retrieve data for registry value InstallPath, writes to buffer
	result = RegQueryValueExA(hKey, "InstallPath", nullptr, nullptr, reinterpret_cast<LPBYTE>(steamPath), &bufferSize);
	RegCloseKey(hKey);
	if (result != ERROR_SUCCESS)
	{
		std::cerr << "ERROR: Could not read install path: " << result << '\n';
		return LocateSteamPathManual();
	}


	return std::string(steamPath);
}

std::string Steam::LocateLibraryVDF(const std::string& steamPath)
{
	// construct libraryfolders.vdf path
	std::filesystem::path vdfPath = std::filesystem::path(steamPath) / "steamapps" / "libraryfolders.vdf";
	if (!std::filesystem::exists(vdfPath))
	{
		std::cerr << "ERROR: Could not find libraryfolders.vdf" << '\n';
		std::cout << "Manually input the path to libraryfolders.vdf: " << '\n';
		std::string manualPath;
		std::getline(std::cin, manualPath);

		vdfPath = std::filesystem::path(manualPath);
		if (!std::filesystem::exists(vdfPath))
		{
			return std::string("");
		}
	}

	return vdfPath.string();
}