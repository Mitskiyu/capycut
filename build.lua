workspace "Capycut"
    configurations { "Debug", "Release" }
    platforms { "Win32", "x64" } 
    location "build"

project "Capycut"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}-%{cfg.platform}"

    files { "src/**.cpp", "src/**.hpp" }
    links { "Ole32", "Shell32", "Uuid" }

    filter { "system:windows", "platforms:Win32" }
        systemversion "latest"
        architecture "x86"

    filter { "system:windows", "platforms:x64" }
        systemversion "latest"
        architecture "x86_64"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
