# Tron Battle Tanks
![Made With](https://img.shields.io/badge/Made%20With-C%2B%2B-blue) ![Engine](https://img.shields.io/badge/Engine-Custom%20Engine-orange)
![Gameplay](https://img.shields.io/badge/Gameplay-2D--Platformer-orange)
![Input](https://img.shields.io/badge/input-Keyboard%20%2B%20Controller/XINPUT-lightgrey)
![Status](https://img.shields.io/badge/Status-WIP-darkblue)
![demo2](https://github.com/user-attachments/assets/06298d1d-c79c-4c11-8419-f7d9d7439b25)

![demo](https://github.com/user-attachments/assets/e5893dc1-c3d2-4f9e-ba82-724a6e563998)

![demo3](https://github.com/user-attachments/assets/32ba1b5c-b70a-46a2-ae88-dbb9fb3e23c5)

<details>
  <summary>Secret Level</summary>
  
  ![demo4](https://github.com/user-attachments/assets/4a157f1d-7edb-4777-b399-c6ba8b697588)

</details>

An in-progress 2D game engine built using **C++** and **SDL2**, featuring a **component-based architecture**, game object hierarchy, command-pattern-based input system, and prefab creation.

Inspired by _Game Programming Patterns_ by **Robert Nystrom**, this engine powers **Tron Battle Tanks**, a prototype for a top-down tank battle game.

---

## Project Overview

This project was built from scratch in C++ using SDL2, with a focus on core game engine architecture principles. It serves both as a learning project and as a flexible framework for future 2D games.

---

## Features

### Architecture (SDL2-2.30.7)
- **Scene System**: Manage multiple scenes (main menu, gameplay, etc.) through a common `Scene` interface.
- **GameObject System**: Hierarchical GameObjects with parent-child relationships, tags, and component containers.
- **Component System**: A flexible `Component` base class with support for rendering, updating, and custom logic (`TransformComponent`, `TextureComponent`, etc.).
- **Prefab System**: Quickly instantiate predefined tank prefabs (Red, Blue, Yellow) via a centralized `PrefabRegistry`.

### Input & Commands
- Input system supports both **keyboard** and **gamepad (XInput)** controls.
- Uses the **Command Pattern** to abstract input into flexible actions (`MoveCommand`, `RotateTurretCommand`, etc.).

### Prefabs & Gameplay
- `PrefabRegistry` sets up complete tank GameObjects with textures, health components, box colliders, and child turrets.
- `TestingScene` initializes the battlefield layout, sets up the background, FPS counter, and registers all input bindings.

### Rendering & UI (SDL2_image-2.8.2 / SDL2_ttf-2.22.0)
- 2D rendering support using SDL2 and SDL_ttf.
- `TextureComponent` handles sprite rendering.
- `TextComponent` enables custom text and UI elements (e.g., FPS counter, titles).

### Audio System (SDL2_mixer-2.8.1)
- Audio system accessible via a **Service Locator pattern**.
- Supports music/effects playback and commands like muting via `MuteSoundCommand`.

---

## Tech Stack

- **C++20**
- **SDL2**
- **SDL_image**, **SDL_ttf**
- **glm**
- **XInput** (gamepad support)
- Design Patterns like **Command**, **Service Locator**, **Observer**, **Component**, **Singleton**, **Dirty flag** etc...

---

## Building

**Requirements:** Windows, and Visual Studio 2022 or newer with the **Desktop development with C++** workload.

Everything else is vendored in `vendor/` (SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, glm, Visual Leak Detector) and committed to the repo — there is no package manager or CMake step to run.

1. Clone the repo.
2. Open `FMLEngine.sln` and build (or from a terminal):

```powershell
msbuild FMLEngine.sln /p:Configuration=Debug /p:Platform=x64 /m
```

3. Run `bin\Tron_BattleTanks\x64\Debug\Tron_BattleTanks.exe`.

Configurations are `Debug`/`Release` on `x64` and `x86`. The projects build with whatever MSVC toolset your Visual Studio installation provides (`$(DefaultPlatformToolset)`), so no "Retarget solution" step is needed. To pin a specific toolset instead, pass it explicitly:

```powershell
msbuild FMLEngine.sln /p:Configuration=Debug /p:Platform=x64 /p:PlatformToolset=v143
```

The post-build step copies the SDL DLLs and the `data/` folder next to the executable, so the game must be launched from its output directory (asset paths are relative to the exe).

### Memory leak checking

Debug builds run under [Visual Leak Detector](https://github.com/KindDragon/vld/releases) 2.5.1 and print a leak report to the debugger output on exit. VLD is fully vendored — headers, import libs, and the runtime DLLs (`vld_x64.dll`/`vld_x86.dll` plus `dbghelp.dll`) all live in `vendor/Visual Leak Detector/`, and the post-build step copies them next to the exe along with `vld.ini`. **No local VLD installation is required**, so a fresh clone builds and runs as-is.

It is enabled by the `USE_VLD` preprocessor definition on the Debug configurations. To build Debug without leak detection, remove `USE_VLD` from the Tron_BattleTanks project's **C/C++ → Preprocessor → Preprocessor Definitions**. Release builds never link VLD.

---

## Contact

Feel free to connect with me:
- **GitHub**: [https://github.com/BeHaVeZ/]
- **LinkedIn**: [www.linkedin.com/in/alexanderterentyev/]
