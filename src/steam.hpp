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
	}

	std::string getSteamPath() const { return steamPath; }
	std::string getLibraryVDF() const { return vdfPath; }
	std::vector<std::string> getLibraryPaths() const { return libraryPaths; }
private:
	std::string steamPath;
	std::string vdfPath;
	std::vector<std::string> libraryPaths;

	static std::string LocateSteamPath();
	static std::string LocateLibraryVDF(const std::string& steamPath);
	static std::vector<std::string> LocateLibraryPaths(const std::string& vdfPath);
};

#endif