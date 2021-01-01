#pragma once
#include "stdafx.h"
#include "Settings.h"
#include "Client.h"
#include "console.h"

// Header
// =============
class Clientmanager
{
public:
	Client			clients[10];
	int				clientTotal;
	bool			checkClientExist(DWORD);
	// Clients state
	//-------------------------------------
	int				clientLeaderIndex = 0;
	bool			processIsActive = true;
	bool			autoAttackIsActive = false;

	// Settings
	toolSettings settings;
	// Constructor
	Clientmanager();
	// Helper functions
	void addClient(DWORD);
	void removeClient(int clientNum);
	void removeClients();
	int findClientIndex(int clientNum);
	void printClients();
	void callClientsMethod(void(Client::* funcPtr)());
	void callClientsMethodAsync(void(Client::* funcPtr)());
	// Process Functions
	bool getProcessState();
	void mainProcess();
	void stopProcess();
	// Action Functions
	void toggleAutoFollow();
	void toggleAutoAttack();
	void set(std::string name, int clienNum);
	void showAllClientDetails();
	void showClientDetails(int clientNum);
};
