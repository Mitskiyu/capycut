#ifndef SHORTCUT_HPP
#define SHORTCUT_HPP

#include <string>

#include "steam.hpp"

class Shortcut
{
public:
	Shortcut() : startMenuPath(LocateStartMenuPath()), subDirPath(startMenuPath)
	{
	}

	std::wstring getStartMenuPath() const { return startMenuPath; }
	static std::wstring CreateSubDir(const std::wstring& startMenuPath);
	static void CreateShortcut(const Steam::AppData& data, const std::wstring& output, const std::wstring& iconPath);
private:
	std::wstring startMenuPath;
	std::wstring subDirPath;

	static std::wstring LocateStartMenuPath();
};

#endif