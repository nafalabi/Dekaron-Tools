#pragma once
#include "stdafx.h"

namespace console
{
	extern std::stringstream outputStream;
	void newCommand();
	void print(std::string = "", bool newLine = false);
	void clear();
	std::stringstream getInput();
};

// Body
// ==========

static std::stringstream console::outputStream = std::stringstream("");

inline void console::newCommand()
{
	std::cout << "\n";
	std::cout << "Dekaron > ";
}

inline void console::print(std::string message, bool newLine)
{
	// Add the parameter to the stream
	console::outputStream << message;
	// Output the stream
	std::cout << console::outputStream.str();
	// Clear the stream
	console::outputStream.str("");
	if (newLine) console::newCommand();
}

inline void console::clear()
{
	#if defined _WIN32
		system("cls");
	#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
		system("clear");
	#elif defined (__APPLE__)
		system("clear");
	#endif
}

inline std::stringstream console::getInput()
{
	std::string input;
	// Get the input
	std::getline(std::cin, input);

	return std::stringstream(input);
}
