# Tron Battle Tanks
![Made With](https://img.shields.io/badge/Made%20With-C%2B%2B-blue) ![Engine](https://img.shields.io/badge/Engine-Custom%20Engine-orange)
![Gameplay](https://img.shields.io/badge/Gameplay-2D--Platformer-orange)
![Input](https://img.shields.io/badge/input-Keyboard%20%2B%20Controller/XINPUT-lightgrey)
![Status](https://img.shields.io/badge/Status-WIP-darkblue)


An in-progress 2D game engine built using **C++** and **SDL2**, featuring a modular **component-based architecture**, game object hierarchy, command-pattern-based input system, and prefab creation.

Inspired by _Game Programming Patterns_ by **Robert Nystrom**, this engine powers **Tron Battle Tanks**, a prototype for a top-down tank battle game.

---

## Project Overview

This project was built from scratch in C++ using SDL2, with a focus on reusability, modularity, and learning core game engine architecture principles. It serves both as a learning project and as a flexible framework for future 2D games.

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

## Screenshots

![image](https://github.com/user-attachments/assets/5ac7b09f-7c9e-4e1a-863c-bef85ea5508e)

---

## Contact

Feel free to connect with me:
- **GitHub**: [https://github.com/BeHaVeZ/]
- **LinkedIn**: [www.linkedin.com/in/alexanderterentyev/]
