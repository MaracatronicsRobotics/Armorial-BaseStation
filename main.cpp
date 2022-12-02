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
#include <QSerialPortInfo>

#include <grpc++/health_check_service_interface.h>

#include <Armorial/Utils/ExitHandler/ExitHandler.h>
#include <Armorial/Threaded/EntityManager/EntityManager.h>

#include <src/basestations/real/realbasestation.h>
#include <src/basestations/sim/simbasestation.h>
#include <src/manager/basestationmanager.h>
#include <src/service/basestationservice.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/color.h>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // Setup QCommandLineParser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    // Enable health checking for service
    grpc::EnableDefaultHealthCheckService(true);

    // Setup application options

    // Mode (sim / radio)
    QCommandLineOption actuatorModeOpt("mode",
                                    QCoreApplication::translate("main", "Set actuator mode"),
                                    QCoreApplication::translate("main", "<sim | radio>"));
    parser.addOption(actuatorModeOpt);

    // Serial port name
    QCommandLineOption serialPortOpt("serial",
                                    QCoreApplication::translate("main", "Set serial port"),
                                    QCoreApplication::translate("main", "<serial_port>"));
    parser.addOption(serialPortOpt);

    // Simulator address
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
    QString actuatorMode = "sim";
    QString simAddress = "127.0.0.1";
    QString teamColor = "blue";
    QString serialPortName = "";

    // Parse options

    // Parse actuator mode
    if(parser.isSet(actuatorModeOpt)) {
        // Check if match regex
        QString parsedValue = parser.value(actuatorModeOpt);
        bool match = QRegularExpression("(sim|radio)").match(parsedValue).hasMatch();
        if(!match) {
            spdlog::error("The given actuator mode '{}' does not match the options {}, please check that.", parsedValue.toStdString(), fmt::format(fmt::bg(fmt::terminal_color::red), "sim | radio"));
            exit(-1);
        }
        else {
            actuatorMode = parsedValue;
        }

        // Show available ports if no serial port has been given
        if(parsedValue == "radio" && !parser.isSet(serialPortOpt)) {
            spdlog::error("While using RadioActuator you need to specify a serial port. The available ports are:");

            QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
            if(availablePorts.size()) {
                for (auto &a : availablePorts) {
                    spdlog::info("{}", a.portName().toStdString());
                }
            }

            exit(-1);
        }
    }

    // Parse serial port name
    if(parser.isSet(serialPortOpt)) {
        // Get value
        QString parsedValue = parser.value(serialPortOpt);

        // Check if port is in available ports list
        QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
        bool found = false;
        for (auto &sp : availablePorts) {
            if(sp.portName() == parsedValue) {
                found = true;
                break;
            }
        }

        // If not found, show all available serial ports
        if(!found) {
            spdlog::error("The given serial port '{}' is not available.", parsedValue.toStdString());
            if(availablePorts.size()) {
                spdlog::info("The following ports are: ");
                for (auto &a : availablePorts) {
                    spdlog::info("{}", a.portName().toStdString());
                }
            }
            exit(-1);
        }
        else {
            serialPortName = parsedValue;
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
    else {
        spdlog::warn("There is no sim address provided, so it will run by default at {}.", fmt::format(fmt::bg(fmt::terminal_color::red), simAddress.toStdString()));
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
    else {
        spdlog::warn("There is no team color provided, so it will run by default using {}.", teamColor.toStdString() == "blue" ? fmt::format(fmt::bg(fmt::terminal_color::blue), "blue") : fmt::format(fmt::fg(fmt::terminal_color::black) | fmt::bg(fmt::terminal_color::yellow), "yellow"));
    }

    // Setup ExitHandler
    Utils::ExitHandler::setApplication(&a);
    Utils::ExitHandler::setup();

    // Start BaseStation implementation
    BaseStation* baseStation = nullptr;
    if(actuatorMode == "sim") {
        baseStation = new SimBaseStation(simAddress, teamColor == "blue" ? 10301 : 10302);
    }
    else {
        baseStation = new RealBaseStation();
    }

    // Setup BaseStation manager
    BaseStationManager* baseStationManager = new BaseStationManager(baseStation);

    // Setup BaseStation service
    BaseStationService* baseStationService = new BaseStationService("127.0.0.1", 50051, baseStationManager);

    // Create entity manager
    Threaded::EntityManager *entityManager = new Threaded::EntityManager();
    entityManager->addEntity(baseStationManager);
    entityManager->addEntity(baseStationService);

    // Start entities
    entityManager->startEntities();

    bool exec = a.exec();

    // Stop entities
    entityManager->disableEntities();

    // Delete modules
    delete baseStation;

    return exec;
}
