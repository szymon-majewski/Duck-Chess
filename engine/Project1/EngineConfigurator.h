#pragma once

extern class Engine;

class EngineConfigurator
{
private:

	Engine* engine;

public:

	EngineConfigurator(Engine* engine);

	void Configure(int modifiersCount, char** modifiers);
};

/* List of modifiers (id, possible values, default value):
* -board : true, false : true
* -boardColor : true, false : true
* -posInfo : true, false : true
* -moveInfo : true, false : true
*/