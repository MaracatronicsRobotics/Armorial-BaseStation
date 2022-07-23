/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QRegularExpression>

#include <Armorial/Utils/ExitHandler/ExitHandler.h>
#include <Armorial/Threaded/EntityManager/EntityManager.h>

#include <src/client/actuatorclient.h>
#include <src/actuators/sim/simulationactuator.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/color.h>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // Setup QCommandLineParser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    // Setup application options
    // Service network
    QCommandLineOption serviceNetworkOpt("service",
                                      QCoreApplication::translate("main", "Set service address:port"),
                                      QCoreApplication::translate("main", "<address>:<port>"));
    parser.addOption(serviceNetworkOpt);

    // Mode (sim / real)
    QCommandLineOption actuatorModeOpt("mode",
                                    QCoreApplication::translate("main", "Set actuator mode"),
                                    QCoreApplication::translate("main", "<sim | real>"));
    parser.addOption(actuatorModeOpt);

    // Mode (sim / real)
    QCommandLineOption simAddressOpt("simAddress",
                                  QCoreApplication::translate("main", "Set sim address"),
                                  QCoreApplication::translate("main", "<address>"));
    parser.addOption(simAddressOpt);

    // Team color
    QCommandLineOption teamColorOpt("teamColor",
                                 QCoreApplication::translate("main", "Set team color"),
                                 QCoreApplication::translate("main", "<blue | yellow>"));
    parser.addOption(teamColorOpt);

    // Process parser in application
    parser.process(a);

    // Setup default arguments
    QString serviceNetwork = "127.0.0.1:50051";
    QString actuatorMode = "sim";
    QString simAddress = "127.0.0.1";
    QString teamColor = "blue";

    // Parse options
    // Parse service network
    if(parser.isSet(serviceNetworkOpt)) {
        // Check if match regex
        QString parsedValue = parser.value(serviceNetworkOpt);
        bool match = QRegularExpression("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\:[0-9]{1,5}").match(parsedValue).hasMatch();
        if(!match) {
            spdlog::error("The given service network '{}' does not match the pattern {}, please check that.", parsedValue.toStdString(), fmt::format(fmt::bg(fmt::terminal_color::red), "ip:port"));
            exit(-1);
        }
        else {
            serviceNetwork = parsedValue;
        }
    }

    // Parse actuator mode
    if(parser.isSet(actuatorModeOpt)) {
        // Check if match regex
        QString parsedValue = parser.value(actuatorModeOpt);
        bool match = QRegularExpression("(sim|real)").match(parsedValue).hasMatch();
        if(!match) {
            spdlog::error("The given actuator mode '{}' does not match the options {}, please check that.", parsedValue.toStdString(), fmt::format(fmt::bg(fmt::terminal_color::red), "sim | real"));
            exit(-1);
        }
        else {
            actuatorMode = parsedValue;
        }
    }

    // Parse sim address
    if(parser.isSet(simAddressOpt)) {
        QString parsedValue = parser.value(simAddressOpt);
        bool match = QRegularExpression("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}").match(parsedValue).hasMatch();
        if(!match) {
            spdlog::error("The given sim address '{}' does not refers to a valid ip address, please check that.", parsedValue.toStdString());
            exit(-1);
        }
        else {
            simAddress = parsedValue;
        }
    }

    // Parse team color
    if(parser.isSet(teamColorOpt)) {
        // Check if match regex
        QString parsedValue = parser.value(teamColorOpt);
        bool match = QRegularExpression("(blue|yellow)").match(parsedValue).hasMatch();
        if(!match) {
            spdlog::error("The given team color '{}' does not match the options {} | {}, please check that.", parsedValue.toStdString(), fmt::format(fmt::bg(fmt::terminal_color::blue), "blue"), fmt::format(fmt::fg(fmt::terminal_color::black) | fmt::bg(fmt::terminal_color::yellow), "yellow"));
            exit(-1);
        }
        else {
            teamColor = parsedValue;
        }
    }

    // Setup ExitHandler
    Utils::ExitHandler::setApplication(&a);
    Utils::ExitHandler::setup();

    // Start actuator implementation
    BaseActuator *baseActuator = nullptr;
    if(actuatorMode == "sim") {
        baseActuator = new SimulationActuator(simAddress, teamColor == "blue" ? 10301 : 10302);
    }
    else {
        /// TODO: create implementation for real sim
        spdlog::info("Currently the real actuator is not available, please use the 'sim' option.");
        exit(-1);
        baseActuator = nullptr;
    }

    // Setup actuator client that will communicate with gRPC backend
    ActuatorClient *actuatorClient = new ActuatorClient(serviceNetwork.split(":")[0], serviceNetwork.split(":")[1].toUInt(), baseActuator);

    // Create entity manager
    Threaded::EntityManager *entityManager = new Threaded::EntityManager();
    entityManager->addEntity(actuatorClient);

    // Start entities
    entityManager->startEntities();

    bool exec = a.exec();

    // Stop entities
    entityManager->disableEntities();

    return exec;
}
