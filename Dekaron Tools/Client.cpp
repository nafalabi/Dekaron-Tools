#include "client.h"

Client::Client() {
	processId = 0;
}

Client::Client(DWORD PID)
{
	processId = PID;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, PID);
	hwnd = prochelper::findWindowFromProcessId(processId);
	baseAddr = prochelper::getModuleBaseAddress(PID, L"zdkrc.exe");
	hydrateValues();
	// Check if it's valid process, if it's not then return
	if (maxHealth == 0) return;
	setWindowName();
}

BOOL Client::isClientValid()
{
	if (maxHealth == 0) {
		return false;
	}
	else {
		return true;
	}
}

DWORD Client::getPID()
{
	return processId;
}

VOID Client::hydrateValues()
{
	getCharName(charName);
	maxHealth = getMaxHealth();
	curHealth = getCurHealth();
	maxMana = getMaxMana();
	curMana = getCurMana();
	curSelectedTarget = getCurSelectedTarget();
	charId = getSelfCharId();
}

VOID Client::setWindowName()
{
	SetWindowTextA(hwnd, (LPSTR)charName);
}

VOID Client::sendKeyStroke(int keycode, int interval) {
	PostMessage(hwnd, WM_KEYDOWN, keycode, 1);
	Sleep(interval);
	PostMessage(hwnd, WM_IME_KEYUP, keycode, 1);
}

VOID Client::sendMultipleKeyStrokes(std::vector<int> keys, int interval)
{
	for (size_t i = 0; i < keys.size(); i++) {
		PostMessage(hwnd, WM_IME_KEYDOWN, keys[i], 1);
	}
	Sleep(interval);
	for (size_t i = 0; i < keys.size(); i++) {
		PostMessage(hwnd, WM_IME_KEYUP, keys[i], 1);
	}
}



//=======================================
//	     Values Getter Functions
//=======================================

VOID Client::getCharName(CHAR* outStr)
{
	char charName[16];
	uintptr_t charNameAddr = prochelper::findDMAAddy(hProcess, baseAddr + 0x87A8D4, { 0x4,0x8,0x100,0x4,0x18c });
	ReadProcessMemory(hProcess, (BYTE*)charNameAddr, &charName, sizeof(charName), nullptr);
	for (size_t i = 0; i < sizeof(charName); i++)
	{
		outStr[i] = charName[i];
	}
}

FLOAT Client::getMaxHealth() {
	FLOAT result = 0;
	uintptr_t addr = prochelper::findDMAAddy(hProcess, baseAddr + offsets.charInfoBaseAddr, offsets.maxHealth);
	ReadProcessMemory(hProcess, (BYTE*)addr, &result, sizeof(result), nullptr);
	return result;
}

FLOAT Client::getCurHealth()
{
	FLOAT result = 0;
	uintptr_t addr = prochelper::findDMAAddy(hProcess, baseAddr + offsets.charInfoBaseAddr, offsets.curHealth);
	ReadProcessMemory(hProcess, (BYTE*)addr, &result, sizeof(result), nullptr);
	return result;
}

FLOAT Client::getMaxMana()
{
	FLOAT result = 0;
	uintptr_t addr = prochelper::findDMAAddy(hProcess, baseAddr + offsets.charInfoBaseAddr, offsets.maxMana);
	ReadProcessMemory(hProcess, (BYTE*)addr, &result, sizeof(result), nullptr);
	return result;
}

FLOAT Client::getCurMana()
{
	FLOAT result = 0;
	uintptr_t addr = prochelper::findDMAAddy(hProcess, baseAddr + offsets.charInfoBaseAddr, offsets.curMana);
	ReadProcessMemory(hProcess, (BYTE*)addr, &result, sizeof(result), nullptr);
	return result;
}

int Client::getCurSelectedTarget() {
	int result = 0;
	uintptr_t addr = prochelper::findDMAAddy(hProcess, baseAddr + offsets.selectedPlayerBaseAddr, offsets.selecterPlayer);
	ReadProcessMemory(hProcess, (BYTE*)addr, &result, sizeof(result), nullptr);
	return result;
}

int Client::getSelfCharId()
{
	int Id = 0;
	sendKeyStroke(69); // "E" key code
	Id = getCurSelectedTarget();
	sendKeyStroke(27);
	return Id;
}



//=======================================
//			Process Functions
//=======================================

void Client::healthAutoPot()
{
	if (!settings.autoPotHpEnabled) return;

	float maxHealth = getMaxHealth();
	float curHealth = getCurHealth();
	int percentageHealth = floor(curHealth / maxHealth * 100);
	if (percentageHealth < settings.toleratedHpPercentage) {
		sendKeyStroke(settings.potHpKey);
	}
}

void Client::manaAutoPot()
{
	if (!settings.autoPotMpEnabled) return;

	float maxMana = getMaxMana();
	float curMana = getCurMana();
	int percentageMana = floor(curMana / maxMana * 100);
	if (percentageMana < settings.toleratedMpPercentage) {
		sendKeyStroke(settings.potMpKey);
	}
}

void Client::doAutoAttack()
{
	if (!settings.autoAttackEnabled) return;
	sendMultipleKeyStrokes(settings.keycodeNextTarget);

	for (unsigned int i = 0; i < settings.skillKeys.size(); i++)
	{
		sendKeyStroke(settings.skillKeys[i], 10);
		Sleep(1);
	}
}

void Client::doFollowUser(int userId)
{
	// Point the selected user first to self
	sendMultipleKeyStrokes(settings.keycodeSelfTargeting);

	// Set the user that's going to be followed
	uintptr_t selectedPlayerAddr = prochelper::findDMAAddy(hProcess, baseAddr + 0x87BD74, { 0xc,0x88 });
	WriteProcessMemory(hProcess, (BYTE*)selectedPlayerAddr, &userId, sizeof(userId), nullptr);

	// Send key stroke to do the follow
	sendMultipleKeyStrokes(settings.keycodeFollow);
}
