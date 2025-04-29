# Capycut

Generate Steam shortcuts in bulk with a simple .exe file

> [!NOTE]
>
> Shortcuts do not currently generate with the app-specific icon, and instead use the default Steam icon

> [!WARNING]
>
> Only Windows is currently supported

## Usage

1. Download the latest release from the [Releases](./releases) page
2. Run `capycut.exe`
3. Shortcuts are generated in Start Menu\Programs\Capycut

## Build

1. Make sure you have [Premake 5](https://premake.github.io/) installed
2. In the project dir, run:
   `premake5 --file=build.lua <build>`

   Replace `<build>` with your preferred IDE or build system (e.g., vs2022 for Visual Studio 2022)

## Issues

Feel free to open an issue if you encounter any problems
