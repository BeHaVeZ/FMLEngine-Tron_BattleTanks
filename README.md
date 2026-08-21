# Tron Battle Tanks

![Made With](https://img.shields.io/badge/C%2B%2B20-blue) ![Engine](https://img.shields.io/badge/Engine-Custom%20(FMLEngine)-orange)
![Gameplay](https://img.shields.io/badge/Gameplay-Top--Down%20Tank%20Combat-orange)
![Input](https://img.shields.io/badge/Input-Keyboard%20%2B%20XInput-lightgrey)
![AI](https://img.shields.io/badge/AI-Pathfinding%20%7C%20Ricochet%20Prediction%20%7C%20Difficulty%20Profiles-brightgreen)
![Status](https://img.shields.io/badge/Status-Finished-green)

A top-down tank combat game built on FMLEngine, my own C++20 / SDL2 2D engine. The engine, the gameplay systems and all of the AI are written from scratch. There is no middleware and no navigation or behavior library involved.

Inspired by _Game Programming Patterns_ by Robert Nystrom.

<img width="1590" height="766" alt="AIvsAIDemo" src="https://github.com/user-attachments/assets/d1fb62d4-c71a-492b-9464-6ecea687e827" />

![demo2](https://github.com/user-attachments/assets/06298d1d-c79c-4c11-8419-f7d9d7439b25)

![demo](https://github.com/user-attachments/assets/e5893dc1-c3d2-4f9e-ba82-724a6e563998)

![demo3](https://github.com/user-attachments/assets/32ba1b5c-b70a-46a2-ae88-dbb9fb3e23c5)

![demo4](https://github.com/user-attachments/assets/4a157f1d-7edb-4777-b399-c6ba8b697588)

## Project layout

| Project | Description |
| --- | --- |
| `FMLEngine` | Static library. Scenes, game objects and components, input, collision, audio, navigation, debug tooling. |
| `Tron_BattleTanks` | The game itself. Tank AI, gameplay components, level definitions, UI. |

Both projects compile at `/W4` with warnings treated as errors.

## Game AI

The AI runs entirely on the engine's raycast and collision systems.

<img width="1590" height="766" alt="ALLIEDAIDEMO" src="https://github.com/user-attachments/assets/dd1158f1-e891-432d-9a96-63d7c7cc9ba6" />
<img width="1590" height="766" alt="ALLIEDDODGINGDEMO" src="https://github.com/user-attachments/assets/c2eb4652-b13f-47d0-aeb9-376f7ddcd657" />


### Combat behaviour

The tank controller ([AITankControllerComponent.cpp](Tron_BattleTanks/AITankControllerComponent.cpp)) drives the versus opponent and the allied tank in coop.

* Ricochet prediction. `SimulateShot` traces a bullet through its wall bounces and classifies the result as a miss, a hit on the target, or a hit on itself or an ally. The AI refuses shots that would bounce back into friendlies and actively searches for bank shots: firing solutions that ricochet off walls to reach a target it has no direct line to.
* Threat dodging. Incoming bullets are simulated the same way. The AI computes the impact point and time to impact, picks an escape direction perpendicular to the shot and commits to it instead of jittering between directions.
* Stances. The controller switches between pressing, holding range and flanking based on distance and how fast the target is closing in, so different difficulties feel different rather than just aiming better.
* Difficulty profiles. [AIDifficultyProfile.h](Tron_BattleTanks/AIDifficultyProfile.h) is a plain struct of 16 tunables (reaction delay, aim error, lead prediction, dodge commitment, bank shot search parameters and so on) with easy, normal and hard presets. Balancing means editing numbers, not logic.

### Navigation

* [NavGrid](FMLEngine/NavGrid.h) builds a grid from the level's wall colliders and precomputes a clearance field, so path queries take the agent's radius into account and wide tanks never plan through gaps they cannot fit. It also supports dynamic occupancy and random goal picking for patrols.
* [PathFollower](FMLEngine/PathFollower.h) handles periodic replanning, off-path detection and short diverts around blockers without throwing away the whole plan.
* [AgentAvoidance](FMLEngine/AgentAvoidance.h) resolves agent versus agent conflicts with priority based right of way (a chasing agent beats a patrolling one), so NPCs steer around each other instead of stacking up.
* [EnemyPerception](Tron_BattleTanks/EnemyPerception.h) does raycast line of sight checks plus ally-in-line-of-fire tests, which stopped enemies from shooting each other in the back.

<img width="1590" height="888" alt="MeVSAI" src="https://github.com/user-attachments/assets/b0133090-4728-45bb-92e8-d4096c801cbe" />


### Debug overlay

Tuning AI without seeing what it "thinks" is guesswork, so the engine has a visualization overlay ([DebugOverlay.h](FMLEngine/DebugOverlay.h)) with nine independently toggleable channels:

| Key | Channel | Key | Channel | Key | Channel |
| --- | --- | --- | --- | --- | --- |
| 1 | Nav grid | 4 | Avoidance | 7 | Colliders |
| 2 | Clearance | 5 | Perception | 8 | Stats |
| 3 | Paths | 6 | Agent state | 9 | Prediction |

<!-- GIF PLACEHOLDER: cycling debug overlay channels during a fight (nav grid, paths, avoidance, prediction) -->

There are also two dedicated debug scenes, reachable from any level:

* F7 opens a showcase scene where a hard AI ally fights endlessly respawning enemies on its own. Useful for recording and for watching long-run behaviour.
* F8 opens a sandbox on any of the three maps with nothing in it but the player. Enemies, allies, target dummies, bullets and effects can be placed with the mouse, and full waves or the regular spawner can be triggered on demand.

## Gameplay

<!-- GIF PLACEHOLDER: coop run with allied AI and the end of run stats screen -->

* Three modes: a solo campaign (three levels plus a secret one), coop with an allied AI tank and a per-player score split, and versus against an AI opponent.
* Game feel: screen shake on hits and deaths, floating score popups per kill, and a short invulnerability window with a visual flicker after respawning.
* UI flow: a paged main menu with a settings screen, an in-game pause menu, high score entry and an end of run stats screen for coop.
* Scoring is event driven. Damage, deaths, score and level progression all go through the engine's observer system, so scoring rules never live inside collision code.
* Levels are data driven. Each level is a small config (background, collision file, music, spawns, enemy caps) and adding one is a one-line scene subclass.

## Engine

* Hierarchical game objects with parent-child transforms, tags and component containers. Destruction is deferred and event based.
* Scene system with queued scene changes and re-entrant initialization. Composed entities (tanks, bullets, explosions, UI) are assembled in a single prefab registry.
* Command pattern input over keyboard and XInput, with bindings declared in one place and rebuilt per scene.
* AABB collision with static and trigger flags, plus the raycast helpers that power AI perception and shot simulation.
* Service locator based audio, and the usual suspects from Game Programming Patterns where they fit: command, observer, component, dirty flag.

Dependencies: SDL2 (2.30.7), SDL2_image, SDL2_ttf, SDL2_mixer, glm and XInput. All vendored, see below.

## Building

Requirements: Windows and Visual Studio 2022 or newer with the Desktop development with C++ workload.

Everything else is vendored in `vendor/` (SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, glm, Visual Leak Detector) and committed to the repo. There is no package manager or CMake step to run.

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

Debug builds run under [Visual Leak Detector](https://github.com/KindDragon/vld/releases) 2.5.1 and print a leak report to the debugger output on exit. VLD is fully vendored, including the runtime DLLs, so no local installation is required and a fresh clone builds and runs as-is.

Leak detection is enabled by the `USE_VLD` preprocessor definition on the Debug configurations. Remove it to build Debug without VLD. Release builds never link it.

## Contact

- GitHub: [BeHaVeZ](https://github.com/BeHaVeZ/)
- LinkedIn: [alexanderterentyev](https://www.linkedin.com/in/alexanderterentyev/)
