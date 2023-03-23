#pragma once

class Engine;

class PlayerInputManager
{
	Engine& engine;

public:

	PlayerInputManager(Engine& engine);
	void ReadAndManageInput();

private:

	bool ManageMove();
	bool ManageUndo();
	bool ManageEval();
	bool ManageMoveC();

	void PrintWinnerEvaluation();
};