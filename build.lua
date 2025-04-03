workspace "Capycut"
    configurations { "Debug", "Release" }
    location "build"

project "Capycut"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    files { "src/**.cpp", "src/**.hpp" }
    links { "Ole32", "Shell32", "Uuid" }

    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"