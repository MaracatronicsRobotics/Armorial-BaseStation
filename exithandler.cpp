#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "exithandler.h"
#include <iostream>
#include <signal.h>

QCoreApplication* ExitHandler::_app = NULL;

int ExitHandler::_counter = 0;
void ExitHandler::setup(QCoreApplication *app) {
    ExitHandler::_app = app;
    // Setup CTRL+C hook
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ExitHandler::run;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

void ExitHandler::run(int s) {
    ExitHandler::_counter++;
    switch(ExitHandler::_counter) {
        case 1: {
            std::cout << "\n[EXIT HANDLER] Closing Armorial-Actuator...\n";
            // Close application
            ExitHandler::_app->exit();
        } break;
        default:
        case 2: {
            std::cout << "\n[EXIT HANDLER] Halting Armorial-Actuator...\n";
            exit(EXIT_FAILURE);
        } break;
    }
}
