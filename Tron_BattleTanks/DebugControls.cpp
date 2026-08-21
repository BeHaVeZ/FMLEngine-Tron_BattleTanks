#include "DebugControls.h"
#include "DebugOverlay.h"
#include "GodMode.h"
#include "AIDifficultyProfile.h"
#include <iomanip>
#include <iostream>

namespace FML
{
	namespace
	{
		constexpr const char* headingColor = "\033[36m";
		constexpr const char* keyColor = "\033[33m";
		constexpr const char* onColor = "\033[32m";
		constexpr const char* resetColor = "\033[0m";

		void Heading(const char* text)
		{
			std::cout << '\n' << headingColor << text << resetColor << '\n';
		}

		void Entry(const char* keys, const char* description)
		{
			std::cout << "  " << keyColor << std::left << std::setw(16) << keys << resetColor << description << '\n';
		}
	}

	void DebugControls::PrintControls()
	{
		Heading("=== PLAYER 1 ===");
		Entry("W A S D", "Move");
		Entry("Q / E", "Rotate turret");
		Entry("Space", "Shoot");
		Entry("R", "Damage self");

		Heading("=== PLAYER 2 ===");
		if (GameData::Player2IsAI)
		{
			Entry("AI", DifficultyName(GameData::AiDifficulty));
		}
		else
		{
			Entry("Arrow keys", "Move");
			Entry("[ / ]", "Rotate turret");
			Entry("Right Ctrl", "Shoot");
			Entry("P", "Damage self");
		}

		Heading("=== GAMEPAD ===");
		Entry("D-Pad", "Move");
		Entry("LB / RB", "Rotate turret");
		Entry("A", "Shoot / confirm");
		Entry("Y", "Mute");
		Entry("Start", "Pause");

		Heading("=== MENUS ===");
		Entry("Esc", "Pause / resume");
		Entry("W S / Up Down", "Move selection");
		Entry("Enter / Space", "Activate");

		Heading("=== AUDIO ===");
		Entry("M / F2", "Toggle mute");
		Entry("- / =", "Volume down / up");

		Heading("=== DEBUG ===");
		Entry("F1", "Skip to next level");
		Entry("F3", "Toggle debug overlay");
		Entry("F4", "Cycle focused agent");
		Entry("F5", "Reload current scene");
		Entry("F6", GodMode::IsEnabled() ? "Toggle god mode (currently ON)" : "Toggle god mode (currently OFF)");
		Entry("F7", "AI showcase scene (hard AI ally vs enemies)");
		Entry("F8", "Sandbox scene (spawn anything by hand)");

		Heading("=== SANDBOX SCENE (F8) ===");
		Entry("Left / Right", "Cycle spawn type (tanks, ally, dummy, bullet, FX)");
		Entry("U", "Cycle AI ally difficulty");
		Entry("Right click", "Spawn selected at cursor (snaps to walkable)");
		Entry("Left click", "Teleport player to cursor");
		Entry("Tab", "Cycle map (level00 / 01 / 02)");
		Entry("T", "Spawn an enemy wave");
		Entry("G", "Toggle enemy auto-spawning");
		Entry("C", "Clear spawned NPCs");
		Entry("X", "Clear bullets");

		Heading("=== OVERLAY CHANNELS (enable F3 first) ===");
		for (const DebugChannelInfo& info : DebugChannelInfos)
		{
			std::cout << "  " << keyColor << std::left << std::setw(16) << info.keyLabel << resetColor << info.name;
			if (DebugOverlay::Instance().IsChannelSelected(info.channel))
			{
				std::cout << onColor << "  [on]" << resetColor;
			}
			std::cout << '\n';
		}

		std::cout << std::endl;
	}
}
