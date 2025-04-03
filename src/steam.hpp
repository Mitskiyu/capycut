#ifndef STEAM_HPP
#define STEAM_HPP

#include <string>

class Steam {
public:
	Steam()
		: steamPath(LocateSteamPath()), vdfPath(LocateLibraryVDF(steamPath))
	{
	}

	std::string getSteamPath() const { return steamPath; }
	std::string getLibraryVDF() const { return vdfPath; }
private:
	std::string steamPath;
	std::string vdfPath;

	static std::string LocateSteamPath();
	static std::string LocateLibraryVDF(const std::string& steamPath);
};

#endif