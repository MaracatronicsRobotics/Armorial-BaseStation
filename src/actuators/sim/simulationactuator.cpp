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


#include "simulationactuator.h"

#include <spdlog/spdlog.h>

SimulationActuator::SimulationActuator(QString serverAddress, quint16 serverPort) : Base::UDP::Client(serverAddress, serverPort) {
    bool connected = connectToNetwork();
    if(connected) {
        spdlog::info("[{}] Connected to simulator at address '{}' and port '{}'.", typeid(*this).name(), serverAddress.toStdString(), serverPort);
    }
    else {
        spdlog::warn("[{}] Could not connect to simulator at address '{}' and port '{}'.", typeid(*this).name(), serverAddress.toStdString(), serverPort);
    }
}

void SimulationActuator::sendData(const Armorial::ControlPacket& packet) {
    // Creating packet
    RobotControl simulationPacket;

    // Creating robot commands (vel, kick, dribble)
    RobotCommand *command = simulationPacket.add_robot_commands();

    // Setting commands
    // Player id
    command->set_id(packet.robotidentifier().robotid());

    // Player velocity
    RobotMoveCommand *move = command->mutable_move_command();
    MoveLocalVelocity *localVelocity = move->mutable_local_velocity();
    localVelocity->set_forward(packet.robotvelocity().vx());
    localVelocity->set_left(packet.robotvelocity().vy());
    localVelocity->set_angular(packet.robotangularspeed().vw());

    // Player kick speed
    command->set_kick_speed(packet.robotkick().kickspeed());
    command->set_kick_angle(packet.robotkick().kickangle());

    // Player dribble
    command->set_dribbler_speed(packet.dribbling() ? 100 : 0);

    // Parse to datagram
    QByteArray buffer(simulationPacket.ByteSize(), 0);
    simulationPacket.SerializeToArray(buffer.data(), buffer.size());
    QNetworkDatagram datagram(buffer);

    // Try to send data to simulator
    bool sentDatagram = sendDatagram(datagram);
    if(!sentDatagram) {
        spdlog::warn("[{}] Failed to sent datagram to simulator. Is simulator running?", typeid(*this).name());
    }
}

void SimulationActuator::sendZeroData(const Armorial::RobotIdentifier& robotIdentifier) {
    // Creating packet
    RobotControl simulationPacket;

    // Creating robot commands (vel, kick, dribble)
    RobotCommand *command = simulationPacket.add_robot_commands();

    // Setting commands
    // Player id
    command->set_id(robotIdentifier.robotid());

    // Player velocity
    RobotMoveCommand *move = command->mutable_move_command();
    MoveLocalVelocity *localVelocity = move->mutable_local_velocity();
    localVelocity->set_forward(0.0);
    localVelocity->set_left(0.0);
    localVelocity->set_angular(0.0);

    // Player kick speed
    command->set_kick_speed(0.0);
    command->set_kick_angle(0.0);

    // Player dribble
    command->set_dribbler_speed(0);

    // Parse to datagram
    QByteArray buffer(simulationPacket.ByteSize(), 0);
    simulationPacket.SerializeToArray(buffer.data(), buffer.size());
    QNetworkDatagram datagram(buffer);

    // Try to send data to simulator
    bool sentDatagram = sendDatagram(datagram);
    if(!sentDatagram) {
        spdlog::warn("[{}] Failed to sent datagram to simulator. Is simulator running?", typeid(*this).name());
    }
}
