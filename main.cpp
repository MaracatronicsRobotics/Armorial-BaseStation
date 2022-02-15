#include <QCoreApplication>

#include <src/exithandler.h>
#include <src/constants/constants.h>
#include <src/actuator/simactuator/simactuator.h>
#include <src/entities/actuatorclient/actuatorclient.h>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    ExitHandler::setApplication(&a);
    ExitHandler::setup();

    Constants *constants = new Constants("/src/constants/constants.json");

    Color teamColor;
    teamColor.set_isblue(false); // constante

    BaseActuator *actuator = new SimActuator("127.0.0.1", 20011, teamColor, constants); // constante
    ActuatorClient *actuatorClient = new ActuatorClient("localhost", 50054, actuator, constants); // constante

    actuatorClient->start();

    bool exec = a.exec();

    actuatorClient->stopEntity();
    actuatorClient->wait();

    delete actuator;
    delete actuatorClient;
    delete constants;

    return exec;
}
