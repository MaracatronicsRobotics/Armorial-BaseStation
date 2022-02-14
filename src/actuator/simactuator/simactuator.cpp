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

#include "simactuator.h"

#include <src/utils/text/text.h>

SimActuator::SimActuator(QString actuatorAddress, quint16 actuatorPort, Color teamColor) {
    _actuatorAddress = actuatorAddress;
    _actuatorPort = actuatorPort;
    _teamColor = teamColor;

    connectToNetwork();
}

SimActuator::~SimActuator() {
    // Send zero-packet to each robot in the field
    for(int robotId = 0; robotId <= 11; robotId++) {
        sendZeroData(robotId);
    }

    finishConnection();
}

void SimActuator::sendData(ControlPacket data) {
    // Creating packet
    grSim_Packet packet;

    // Setting macro informations (team and timestamp)
    packet.mutable_commands()->set_isteamyellow(!data.robotidentifier().robotcolor().isblue());
    packet.mutable_commands()->set_timestamp(0.0);

    // Creating robot commands (vel, kick, dribble)
    grSim_Robot_Command *command = packet.mutable_commands()->add_robot_commands();

    // Setting commands
    // Player id
    command->set_id(data.robotidentifier().robotid());

    // Player velocity
    command->set_wheelsspeed(false);
    command->set_veltangent(data.robotvelocity().vx());
    command->set_velnormal(data.robotvelocity().vy());
    command->set_velangular(data.robotangularspeed().vw());

    // Player kick speed
    command->set_kickspeedx(data.kickspeed().vx());
    command->set_kickspeedz(data.kickspeed().vz());

    // Player dribble
    command->set_spinner(data.dribbling());

    // Sending data to simulator
    std::string buffer;
    packet.SerializeToString(&buffer);
    if(_actuatorClient->write(buffer.c_str(), buffer.length()) == -1) {
        std::cout << Text::cyan("[ACTUATOR] " , true) << Text::red("Failed to write to socket: ", true) << Text::red(_actuatorClient->errorString().toStdString() + ". Is grSim running?", true) + '\n';
    }
}

void SimActuator::sendZeroData(int robotId) {
    // Creating robot identifier
    RobotIdentifier *robotIdentifier = new RobotIdentifier();
    Color *robotColor = new Color();
    robotColor->set_isblue(_teamColor.isblue());
    robotIdentifier->set_allocated_robotcolor(robotColor);
    robotIdentifier->set_robotid(robotId);

    ControlPacket packet;
    packet.set_allocated_robotidentifier(robotIdentifier);

    sendData(packet);
}

void SimActuator::connectToNetwork() {
    _actuatorClient = new QUdpSocket();

    if(_actuatorClient->isOpen()) {
        _actuatorClient->close();
    }

    _actuatorClient->connectToHost(_actuatorAddress, _actuatorPort, QIODevice::WriteOnly, QAbstractSocket::IPv4Protocol);
}

void SimActuator::finishConnection() {
    if(_actuatorClient->isOpen()) {
        _actuatorClient->close();
    }

    delete _actuatorClient;
}
