#pragma once
#include "stdafx.h"
#include "console.h"
#include "switch.h" // https://github.com/oklas/switch
#include "prochelper.h"
#include "Clientmanager.h"

// Header
// ===========

class Keypress {
private:
	Clientmanager* clientmanager;
public:
	Keypress();
	Keypress(Clientmanager* cm);
	void mainThread();
	bool isKeyPressed(std::vector<int>);
};

// Body
// ============


inline Keypress::Keypress() {
	Keypress::clientmanager = 0;
}

inline Keypress::Keypress(Clientmanager* cm) {
	Keypress::clientmanager = cm;
}

inline void Keypress::mainThread()
{
	while (clientmanager->getProcessState()) {
		// Auto Follow Toggler
		if (isKeyPressed(clientmanager->settings.autoFollowKey)) {
			clientmanager->toggleAutoFollow();
			Sleep(50);
		}
		// Auto Attack Toggler
		else if (isKeyPressed(clientmanager->settings.autoAttackKey)) {
			clientmanager->toggleAutoAttack();
			Sleep(50);
		}
		Sleep(50);
	}
}

inline bool Keypress::isKeyPressed(std::vector<int> keys)
{
	for (size_t i = 0; i < keys.size(); i++) {
		// the "& 0x8000" is bitwise operation to filter the value from false positive result
		if (!(GetAsyncKeyState(keys[i]) & 0x8000)) return false;
	}
	return true;
}