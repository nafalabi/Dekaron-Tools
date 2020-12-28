#pragma once
#include <iostream>
#include "Input.h"
#include "Clientmanager.h"

int main()
{
    Clientmanager * clientmanager = new Clientmanager();
    Input * input = new Input(clientmanager);

    std::thread inputThread = std::thread(&Input::startThread, input);
    std::thread mainProcess = std::thread(&Clientmanager::mainProcess, clientmanager);

    inputThread.join();
    mainProcess.join();
}