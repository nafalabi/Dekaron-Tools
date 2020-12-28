#pragma once
#include "stdafx.h"
#include "console.h"
#include "switch.h" // https://github.com/oklas/switch
#include "prochelper.h"
#include "Clientmanager.h"

// Header
// ===============

class CLI {
private:
	Clientmanager* clientmanager;
	void add(std::stringstream* args);
	void remove(std::stringstream* args);
	void list(std::stringstream* args);
	void detail(std::stringstream* args);
	void set(std::stringstream* args);
	void printProcs(bool onlyDekaron = false);
public:
	CLI();
	CLI(Clientmanager* cm);
	void mainThread();
};

// Body
// ===============

inline CLI::CLI() {
	clientmanager = 0;
}

inline CLI::CLI(Clientmanager* cm) {
	CLI::clientmanager = cm;
}

inline void CLI::mainThread() {
	while (true)
	{
		console::newCommand();
		std::stringstream args = console::getInput();

		// Get the command
		std::string command;
		args >> command;

		SWITCH(command)
			CASE("add") FALL
			CASE("addclient")
				CLI::add(&args); BREAK
			CASE("remove") FALL
			CASE("removeclient")
				CLI::remove(&args); BREAK
			CASE("ls") FALL
			CASE("list") FALL
			CASE("listclients")
				CLI::list(&args); BREAK
			CASE("detail") FALL
			CASE("detailclients")
				CLI::detail(&args); BREAK
			CASE("set") 
				CLI::set(&args); BREAK
			CASE("procs") 
				CLI::printProcs(true); BREAK
			CASE("allprocs") 
				CLI::printProcs(); BREAK
			CASE("clear") 
				console::clear(); BREAK
			CASE("exit") 
				CLI::clientmanager->stopProcess();
				break; BREAK
			DEFAULT console::print("Command not found \n");
		END
	}
}

inline void CLI::add(std::stringstream* args) {
	std::string arg1;
	*args >> arg1;

	if (arg1 == "all") {
		procs procList = prochelper::getProcs(true);
		for (auto pr : procList) {
			clientmanager->addClient(pr.first);
		}
	}
	else {
		int PID = std::atoi(arg1.c_str());
		clientmanager->addClient(PID);
	}
}

inline void CLI::remove(std::stringstream* args)
{
	std::string arg1;
	*args >> arg1;

	if (arg1 == "all") {
		clientmanager->removeClients();
	}
	else {
		int clientNum = std::atoi(arg1.c_str());
		clientmanager->removeClient(clientNum);
	}
}

inline void CLI::list(std::stringstream* args)
{
	clientmanager->printClients();
}

inline void CLI::detail(std::stringstream* args)
{
	std::string arg1;
	*args >> arg1;

	if (arg1 == "all") {
		clientmanager->showAllClientDetails();
	}
	else {
		int clientNum = atoi(arg1.c_str());
		clientmanager->showClientDetails(clientNum);
	}
}

inline void CLI::set(std::stringstream* args) {
	std::string arg1;
	int arg2;
	*args >> arg1;
	*args >> arg2;
	clientmanager->set(arg1, arg2);
}

inline void CLI::printProcs(bool onlyDekaron)
{
	procs procList = prochelper::getProcs(onlyDekaron);

	for (unsigned int i = 0; i < procList.size(); i++)
	{
		proc currentProc = procList[i];
		console::outputStream << "PID ";
		console::outputStream << currentProc.first;
		console::outputStream << " (";
		console::outputStream << currentProc.second;
		console::outputStream << ") \n";
		console::print();
	}
}
