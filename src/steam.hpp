#ifndef STEAM_HPP
#define STEAM_HPP

#include <string>
#include <vector>

class Steam {
public:
	struct AppData {
		int appId{};
		std::wstring name;
		std::wstring installDir;
	};

	Steam()
		: steamPath(LocateSteamPath()),
		vdfPath(LocateLibraryVDF(steamPath)),
		libraryPaths(LocateLibraryPaths(vdfPath))
	{
		for (const auto& lib : libraryPaths)
		{
			std::vector<std::wstring> appManifests = LocateAppManifestPaths(lib);
			appManifestPaths.insert(appManifestPaths.end(), appManifests.begin(), appManifests.end());
		}

		for (const auto& app : appManifestPaths)
		{
			apps.push_back(ParseAppManifest(app));
		}
	}

	std::wstring getSteamPath() const { return steamPath; }
	std::wstring getLibraryVDF() const { return vdfPath; }
	std::vector<std::wstring> getLibraryPaths() const { return libraryPaths; }
	std::vector<std::wstring> getAppManifestPaths() const { return appManifestPaths; }
	std::vector<AppData> getAppsData() const { return apps; }
private:
	std::wstring steamPath;
	std::wstring vdfPath;
	std::vector<std::wstring> libraryPaths;
	std::vector<std::wstring> appManifestPaths;
	std::vector<AppData> apps;

	static std::wstring LocateSteamPath();
	static std::wstring LocateLibraryVDF(const std::wstring& steamPath);
	static std::vector<std::wstring> LocateLibraryPaths(const std::wstring& vdfPath);
	static std::vector<std::wstring> LocateAppManifestPaths(const std::wstring& libraryPath);
	static AppData ParseAppManifest(const std::wstring& appManifestPath);
};

#endif