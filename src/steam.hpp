#ifndef STEAM_HPP
#define STEAM_HPP

#include <string>

class Steam {
public:
	Steam()
		: steamPath(LocateSteamPath())
	{
	}

	std::string getSteamPath() const { return steamPath; }
private:
	std::string steamPath;

	static std::string LocateSteamPath();
};

#endif