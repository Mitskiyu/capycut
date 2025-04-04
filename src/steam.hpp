#ifndef STEAM_HPP
#define STEAM_HPP

#include <string>
#include <vector>

class Steam {
public:
	Steam()
		: steamPath(LocateSteamPath()),
		vdfPath(LocateLibraryVDF(steamPath)),
		libraryPaths(LocateLibraryPaths(vdfPath))
	{
		for (const auto& lib : libraryPaths)
		{
			std::vector<std::string> appManifests = LocateAppManifestPaths(lib);
			appManifestPaths.insert(appManifestPaths.end(), appManifests.begin(), appManifests.end());
		}
	}

	std::string getSteamPath() const { return steamPath; }
	std::string getLibraryVDF() const { return vdfPath; }
	std::vector<std::string> getLibraryPaths() const { return libraryPaths; }
	std::vector<std::string> getAppManifestPaths() const { return appManifestPaths; }
private:
	std::string steamPath;
	std::string vdfPath;
	std::vector<std::string> libraryPaths;
	std::vector<std::string> appManifestPaths;

	static std::string LocateSteamPath();
	static std::string LocateLibraryVDF(const std::string& steamPath);
	static std::vector<std::string> LocateLibraryPaths(const std::string& vdfPath);
	static std::vector<std::string> LocateAppManifestPaths(const std::string& libraryPath);
};

#endif