#pragma once

class ConsolePrinterHandler
{
private:

	ConsolePrinterHandler* successor;

public:

	void SetSuccessor(ConsolePrinterHandler* successor);
	void Handle();

	//virtual void Print() = 0;
};