#include "Clientmanager.h"

// Constructor
Clientmanager::Clientmanager()
{
	clientTotal = 0;
}

//=================================================
//				Helper functions
//=================================================

bool Clientmanager::checkClientExist(DWORD PID)
{
	for (int i = 0; i < clientTotal; i++)
	{
		if (clients[i].getPID() == PID)
		{
			return true;
		}
	}
	return false;
}

void Clientmanager::addClient(DWORD PID)
{
	std::stringstream ss;

	if (checkClientExist(PID))
	{
		ss << "PID " << PID << " is already added.\n";
		console::print(ss.str());
		return;
	}

	// Initiate a client
	clients[clientTotal] = Client(PID);
	// if the client is invalid return
	if (!clients[clientTotal].isClientValid())
	{
		ss << "The client for PID " << PID << " is invalid, make sure you have logged in to your character.\n";
		console::print(ss.str());
		return;
	}

	clientTotal += 1;
	ss << "PID " << PID << " has been added.\n";
	console::print(ss.str());
}

void Clientmanager::removeClient(int clientNum)
{
	int index = findClientIndex(clientNum);
	int clientListSize = sizeof(clients) / sizeof(Client);
	if (index == -1)
		return;
	if (index >= clientListSize)
		return;
	int PID = clients[index].getPID();

	// Delete the client from the list
	for (int i = index; i < clientListSize - 1; i++)
		clients[i] = clients[i + 1];
	// shrink the total number of clients
	clientTotal -= 1;

	std::stringstream ss;
	ss << "Client PID " << PID << " has been removed" << index << "\n";
	console::print(ss.str());
}

void Clientmanager::removeClients()
{
	clientTotal = 0;

	std::stringstream ss;
	ss << "All the clients have been removed\n";
	console::print(ss.str());
}

int Clientmanager::findClientIndex(int clientNum)
{
	for (int i = 0; i < clientTotal; i++)
	{
		if (clients[i].getPID() == clientNum)
			return i;
	}

	if (clientNum <= 0 || clientNum > clientTotal)
		return -1;

	return clientNum - 1;
}

void Clientmanager::printClients()
{
	std::stringstream message;

	for (int i = 0; i < clientTotal; i++)
	{
		clients[i].hydrateValues();
		message << "\t" << (i + 1) << ". " << (i == clientLeaderIndex ? "*" : "");
		message << clients[i].charName;
		message << " PID " << clients[i].getPID() << "\n";
	}

	console::print(message.str());
}

void Clientmanager::callClientsMethod(void (Client::* funcPtr)())
{
	void (Client:: * function)();
	function = funcPtr;
	for (int i = 0; i < clientTotal; i++)
	{
		(clients[i].*function)();
	}
}

void Clientmanager::callClientsMethodAsync(void (Client::* funcPtr)())
{
	void (Client:: * func)();
	func = funcPtr;

	for (int i = 0; i < clientTotal; i++)
	{
		// auto dump = std::async(function, clients[i]);
		std::thread attack(func, clients[i]);
		attack.detach();
		Sleep(1);
	}
	Sleep(clients[0].settings.skillKeys.size() * 10);
}

//=================================================
//			   Process functions
//=================================================

bool Clientmanager::getProcessState()
{
	return processIsActive;
}

void Clientmanager::mainProcess()
{
	while (processIsActive)
	{
		callClientsMethod(&Client::manaAutoPot);
		callClientsMethod(&Client::healthAutoPot);

		if (autoAttackIsActive)
		{
			callClientsMethodAsync(&Client::doAutoAttack);
		}
		Sleep(1);
	}
}

void Clientmanager::stopProcess()
{
	processIsActive = false;
}

//=================================================
//				Action functions
//=================================================

void Clientmanager::toggleAutoFollow()
{
	// Turn off auto attack
	autoAttackIsActive = false;

	// Get Leader Id
	int leaderId = clients[clientLeaderIndex].charId;

	for (int i = 0; i < clientTotal; i++)
	{
		if (i != clientLeaderIndex)
		{
			clients[i].doFollowUser(leaderId);
		}
	}

	console::outputStream << "All clients is following ";
	console::outputStream << clients[clientLeaderIndex].charName;
	console::print(console::outputStream.str());
}

void Clientmanager::toggleAutoAttack()
{
	autoAttackIsActive = !autoAttackIsActive;

	console::outputStream << "Auto attack is ";
	console::outputStream << (autoAttackIsActive ? "ON" : "OFF");
	console::print(console::outputStream.str());
}

void Clientmanager::set(std::string name, int clienNum)
{
	int index = findClientIndex(clienNum);
	if (index == -1)
		return;

	if (name == "leader")
	{
		clientLeaderIndex = index;
		console::outputStream << "Leader Has been set to ";
		console::outputStream << clients[clientLeaderIndex].charName;
		console::print(console::outputStream.str());
	}
	else if (name == "autoHP")
	{
		console::print("input 1 to enable it and 0 to disable it, [1/0]");
		console::getInput();
		bool value;
		console::getInput() >> value;
		clients[index].settings.autoPotHpEnabled = value;
	}
	else if (name == "autoMP")
	{
		console::print("input 1 to enable it and 0 to disable it, [1/0]");
		bool value;
		console::getInput() >> value;
		clients[index].settings.autoPotMpEnabled = value;
	}
	else if (name == "autoAttack")
	{
		console::print("input 1 to enable it and 0 to disable it, [1/0]");
		bool value;
		console::getInput() >> value;
		clients[index].settings.autoAttackEnabled = value;
	}
	else if (name == "HPPercentge")
	{
	}
	else if (name == "MPPercentage")
	{
	}
	else if (name == "potHPKey")
	{
	}
	else if (name == "potMPKey")
	{
	}
	else if (name == "skillKeys")
	{
	}
	else if (name == "followKey")
	{
	}
	else if (name == "selfTargetKey")
	{
	}
	else if (name == "nextTargetKey")
	{
	}
}

void Clientmanager::showAllClientDetails()
{
	for (int i = 0; i < clientTotal; i++)
	{
		showClientDetails(i + 1);
		console::print("---------------------------------------\n");
	}
}

void Clientmanager::showClientDetails(int clientNum)
{
	int index = findClientIndex(clientNum);
	Client* cl = &clients[index];
	std::stringstream ss;
	ss << "Name               : " << cl->charName << "\n";
	ss << "autoHP             : " << (cl->settings.autoPotHpEnabled ? "enabled" : "disabled") << "\n";
	ss << "autoMP             : " << (cl->settings.autoPotMpEnabled ? "enabled" : "disabled") << "\n";
	ss << "autoAttack         : " << (cl->settings.autoAttackEnabled ? "enabled" : "disabled") << "\n";
	ss << "auto HP percentage : " << cl->settings.toleratedHpPercentage << "%"
		<< "\n";
	ss << "auto HP percentage : " << cl->settings.toleratedMpPercentage << "%"
		<< "\n";
	console::print(ss.str());
}

void Clientmanager::testFunction()
{
	for (int i = 0; i < clientTotal; i++)
	{
		clients[i].sendKeyStroke(112);
	}
}