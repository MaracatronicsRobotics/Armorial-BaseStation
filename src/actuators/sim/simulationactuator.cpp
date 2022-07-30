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
        spdlog::info("[{}] Connected to simulator at address '{}' and port '{}'.", clientName().toStdString(), serverAddress.toStdString(), serverPort);
    }
    else {
        spdlog::warn("[{}] Could not connect to simulator at address '{}' and port '{}'.", clientName().toStdString(), serverAddress.toStdString(), serverPort);
    }
}

void SimulationActuator::sendData(const Armorial::ControlPacket& packet) {
    // Creating packet
    fira_message::sim_to_ref::Packet pkt;
    fira_message::sim_to_ref::Command *command = pkt.mutable_cmd()->add_robot_commands();

    // Setting macro informations (team and timestamp)
    command->set_id(packet.robotidentifier().robotid());
    command->set_yellowteam(!packet.robotidentifier().robotcolor().isblue());

    // Set wheels speed
    command->set_wheel_left(packet.w2());
    command->set_wheel_right(packet.w1());

    spdlog::info("{} {} {} {}", packet.robotidentifier().robotid(), !packet.robotidentifier().robotcolor().isblue(), packet.w2(), packet.w1());

    // Parse to datagram
    QByteArray buffer(pkt.ByteSize(), 0);
    pkt.SerializeToArray(buffer.data(), buffer.size());
    QNetworkDatagram datagram(buffer);

    // Try to send data to simulator
    bool sentDatagram = sendDatagram(datagram);
    if(!sentDatagram) {
        spdlog::warn("[{}] Failed to sent datagram to simulator. Is simulator running?", clientName().toStdString());
    }
}

void SimulationActuator::sendZeroData(const Armorial::RobotIdentifier& robotIdentifier) {
    // Creating packet
    fira_message::sim_to_ref::Packet pkt;
    fira_message::sim_to_ref::Command *command = pkt.mutable_cmd()->add_robot_commands();

    // Setting macro informations (team and timestamp)
    command->set_id(robotIdentifier.robotid());
    command->set_yellowteam(!robotIdentifier.robotcolor().isblue());

    // Set wheels speed
    command->set_wheel_left(0.0);
    command->set_wheel_right(0.0);

    // Parse to datagram
    QByteArray buffer(pkt.ByteSize(), 0);
    pkt.SerializeToArray(buffer.data(), buffer.size());
    QNetworkDatagram datagram(buffer);

    // Try to send data to simulator
    bool sentDatagram = sendDatagram(datagram);
    if(!sentDatagram) {
        spdlog::warn("[{}] Failed to sent datagram to simulator. Is simulator running?", clientName().toStdString());
    }
}
