#include <QCoreApplication>

#include <src/exithandler.h>
#include <src/constants/constants.h>
#include <src/actuator/simactuator/simactuator.h>
#include <src/entities/actuatorclient/actuatorclient.h>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    ExitHandler::setApplication(&a);
    ExitHandler::setup();

    Constants *constants = new Constants(QString(PROJECT_PATH) + "/src/constants/constants.json");

    Color teamColor;
    teamColor.set_isblue(constants->isTeamBlue());

    BaseActuator *actuator = new SimActuator(constants);
    ActuatorClient *actuatorClient = new ActuatorClient(actuator, constants);

    actuatorClient->start();

    bool exec = a.exec();

    actuatorClient->stopEntity();
    actuatorClient->wait();

    delete actuator;
    delete actuatorClient;
    delete constants;

    return exec;
}
