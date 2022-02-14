#include <QCoreApplication>

#include <src/exithandler.h>
#include <src/actuator/simactuator/simactuator.h>
#include <src/entities/actuatorclient/actuatorclient.h>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    ExitHandler::setApplication(&a);
    ExitHandler::setup();

    Color teamColor;
    teamColor.set_isblue(false);

    BaseActuator *actuator = new SimActuator("127.0.0.1", 20011, teamColor);
    ActuatorClient *actuatorClient = new ActuatorClient("localhost", 50054, actuator);

    actuatorClient->start();

    bool exec = a.exec();

    actuatorClient->stopEntity();
    actuatorClient->wait();

    delete actuator;
    delete actuatorClient;

    return exec;
}
