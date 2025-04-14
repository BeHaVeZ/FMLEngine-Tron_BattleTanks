#include "InputBindingHelper.h"
#include "MoveCommand.h"
#include "RotateTurretCommand.h"
#include "DamageCommand.h"
#include "ShootCommand.h"

namespace FML
{
	void InputBindingHelper::BindGlobalCommands()
	{
		InputHandler::Instance().BindCommand(SDLK_F2, std::make_unique<SkipLevelCommand>(), InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindCommand(SDLK_F5, std::make_unique<ReloadSceneCommand>(), InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindCommand(SDLK_m, std::make_unique<MuteSoundCommand>(), InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_Y, std::make_unique<MuteSoundCommand>(), InputHandler::KeyAction::KeyUp);
	}

	void InputBindingHelper::BindSoloModeControls(GameObject* tank)
	{
		if (!tank) return;

		auto turret = tank->FindChildByTag("Turret");

		InputHandler::Instance().BindCommand(SDLK_w, std::make_unique<MoveCommand>(tank, glm::vec2(0, -1), 100.f));
		InputHandler::Instance().BindCommand(SDLK_s, std::make_unique<MoveCommand>(tank, glm::vec2(0, 1), 100.f));
		InputHandler::Instance().BindCommand(SDLK_a, std::make_unique<MoveCommand>(tank, glm::vec2(-1, 0), 100.f));
		InputHandler::Instance().BindCommand(SDLK_d, std::make_unique<MoveCommand>(tank, glm::vec2(1, 0), 100.f));

		InputHandler::Instance().BindCommand(SDLK_e, std::make_unique<RotateTurretCommand>(turret, 1.f));
		InputHandler::Instance().BindCommand(SDLK_q, std::make_unique<RotateTurretCommand>(turret, -1.f));
		InputHandler::Instance().BindCommand(SDLK_r, std::make_unique<DamageCommand>(tank, 1), InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindCommand(SDLK_SPACE, std::make_unique<ShootCommand>(turret), InputHandler::KeyAction::KeyUp);

		int controllerId = 0;
		InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_UP, std::make_unique<MoveCommand>(tank, glm::vec2(0, -1), 100.f));
		InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_DOWN, std::make_unique<MoveCommand>(tank, glm::vec2(0, 1), 100.f));
		InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_LEFT, std::make_unique<MoveCommand>(tank, glm::vec2(-1, 0), 100.f));
		InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_RIGHT, std::make_unique<MoveCommand>(tank, glm::vec2(1, 0), 100.f));
		InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_RIGHT_SHOULDER, std::make_unique<RotateTurretCommand>(turret, 1.f));
		InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_LEFT_SHOULDER, std::make_unique<RotateTurretCommand>(turret, -1.f));
		InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_A, std::make_unique<ShootCommand>(turret), InputHandler::KeyAction::KeyUp);
	}
	void InputBindingHelper::BindDuoModeControls(GameObject* tank1, GameObject* tank2)
	{
		if (tank1)
		{
			InputHandler::Instance().BindCommand(SDLK_w, std::make_unique<MoveCommand>(tank1, glm::vec2(0, -1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_s, std::make_unique<MoveCommand>(tank1, glm::vec2(0, 1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_a, std::make_unique<MoveCommand>(tank1, glm::vec2(-1, 0), 100.f));
			InputHandler::Instance().BindCommand(SDLK_d, std::make_unique<MoveCommand>(tank1, glm::vec2(1, 0), 100.f));

			InputHandler::Instance().BindCommand(SDLK_e, std::make_unique<RotateTurretCommand>(tank1->FindChildByTag("Turret"), 1.f));
			InputHandler::Instance().BindCommand(SDLK_q, std::make_unique<RotateTurretCommand>(tank1->FindChildByTag("Turret"), -1.f));

			InputHandler::Instance().BindCommand(SDLK_r, std::make_unique<DamageCommand>(tank1, 1), InputHandler::KeyAction::KeyUp);
			InputHandler::Instance().BindCommand(SDLK_SPACE, std::make_unique<ShootCommand>(tank1->FindChildByTag("Turret")), InputHandler::KeyAction::KeyUp);
		}

		if (tank2)
		{
			InputHandler::Instance().BindCommand(SDLK_UP, std::make_unique<MoveCommand>(tank2, glm::vec2(0, -1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_DOWN, std::make_unique<MoveCommand>(tank2, glm::vec2(0, 1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_LEFT, std::make_unique<MoveCommand>(tank2, glm::vec2(-1, 0), 100.f));
			InputHandler::Instance().BindCommand(SDLK_RIGHT, std::make_unique<MoveCommand>(tank2, glm::vec2(1, 0), 100.f));

			InputHandler::Instance().BindCommand(SDLK_RIGHTBRACKET, std::make_unique<RotateTurretCommand>(tank2->FindChildByTag("Turret"), 1.f));
			InputHandler::Instance().BindCommand(SDLK_LEFTBRACKET, std::make_unique<RotateTurretCommand>(tank2->FindChildByTag("Turret"), -1.f));

			InputHandler::Instance().BindCommand(SDLK_p, std::make_unique<DamageCommand>(tank2, 1), InputHandler::KeyAction::KeyUp);
			InputHandler::Instance().BindCommand(SDLK_RCTRL, std::make_unique<ShootCommand>(tank2->FindChildByTag("Turret")), InputHandler::KeyAction::KeyUp);

			int controllerId = 0;
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_UP, std::make_unique<MoveCommand>(tank2, glm::vec2(0, -1), 100.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_DOWN, std::make_unique<MoveCommand>(tank2, glm::vec2(0, 1), 100.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_LEFT, std::make_unique<MoveCommand>(tank2, glm::vec2(-1, 0), 100.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_RIGHT, std::make_unique<MoveCommand>(tank2, glm::vec2(1, 0), 100.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_RIGHT_SHOULDER, std::make_unique<RotateTurretCommand>(tank2->FindChildByTag("Turret"), 1.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_LEFT_SHOULDER, std::make_unique<RotateTurretCommand>(tank2->FindChildByTag("Turret"), -1.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_A, std::make_unique<ShootCommand>(tank2->FindChildByTag("Turret")), InputHandler::KeyAction::KeyUp);
		}

	}
}
