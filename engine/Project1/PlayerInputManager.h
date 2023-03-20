#pragma once

class Engine;

class PlayerInputManager
{
	Engine& engine;

public:

	PlayerInputManager(Engine& engine);
	void ReadAndManageInput();

private:

	void ManageMove();
	void ManageUndo();
	void ManageEval();
	void ManageMoveC();
};