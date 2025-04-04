#ifndef SHORTCUT_HPP
#define SHORTCUT_HPP

#include <string>

class Shortcut
{
public:
	Shortcut() : startMenuPath(LocateStartMenuPath())
	{
	}

	std::wstring getStartMenuPath() const { return startMenuPath; }
private:
	std::wstring startMenuPath;

	static std::wstring LocateStartMenuPath();
};

#endif