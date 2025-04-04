#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

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

std::vector<std::string> Steam::LocateLibraryPaths(const std::string& vdfPath) {
	std::vector<std::string> libraryPaths;
	std::ifstream file(vdfPath);
	if (!file.is_open())
	{
		std::cerr << "ERROR: Could not open: " << vdfPath << '\n';
		return libraryPaths;
	}

	std::regex path(R"x("path"\s+"([^"]+)")x");
	std::string line;
	std::smatch match;

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

std::vector<std::string> Steam::LocateAppManifestPaths(const std::string& libraryPath)
{
	std::vector<std::string> appManifestPaths;

	// construct steamapps path
	std::filesystem::path steamappsPath = std::filesystem::path(libraryPath) / "steamapps";
	if (!std::filesystem::exists(steamappsPath))
	{
		std::cerr << "ERROR: Could not find steamapps dir in library: " << libraryPath << '\n';
		std::cout << "Manually input the 'steamapps' dir path: " << '\n';
		std::string manualPath;
		std::getline(std::cin, manualPath);

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
			std::string filename = filePath.filename().string();

			if (filename.find("appmanifest_") == 0 && filePath.extension() == ".acf")
				appManifestPaths.push_back(filePath.string());
		}
	}

	return appManifestPaths;
}

Steam::AppData Steam::ParseAppManifest(const std::string& appManifestPath)
{
	AppData appData;
	std::fstream file(appManifestPath);
	if (!file.is_open())
	{
		std::cerr << "ERROR: Could not open: " << appManifestPath << '\n';
		return appData;
	}

	std::regex appId(R"x("appid"\s+"(\d+)")x");
	std::regex name(R"x("name"\s+"([^"]+)")x");
	std::regex installDir(R"x("installdir"\s+"([^"]+)")x");
	std::string line;
	std::smatch match;

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