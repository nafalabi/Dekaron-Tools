#pragma once
#include "Clientmanager.h"
#include "CLI.h"
#include "Keypress.h"

// Header
// =====================

class Input {
private:
	Clientmanager* clientManager;
	CLI inputCLI;
	Keypress inputKeypress;
public:
	Input(Clientmanager*);
	void startThread();
};

// Body
// =====================

Input::Input(Clientmanager* cm) {
	Input::clientManager = cm;
	Input::inputCLI = CLI(cm);
	Input::inputKeypress = Keypress(cm);
};

void Input::startThread() {
	std::thread CLIThread(&CLI::mainThread, inputCLI);
	std::thread KeypressThread(&Keypress::mainThread, inputKeypress);
	CLIThread.join();
	KeypressThread.join();
}