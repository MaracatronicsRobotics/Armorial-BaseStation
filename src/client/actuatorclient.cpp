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


#include "actuatorclient.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/color.h>

ActuatorClient::ActuatorClient(QString serviceAddress, quint16 servicePort, BaseActuator *actuator) : Base::GRPC::Client<Armorial::Actuator::ActuatorService>(serviceAddress, servicePort) {
    // Setup actuator implementation
    _actuator = actuator;
}

QString ActuatorClient::name() {
    return "ActuatorClient";
}

void ActuatorClient::initialization() {
    // Try to connect to server
    if(connectToServer(true)) {
        spdlog::info("[{}] Connected to actuator service at address '{}' and port '{}'.", name().toStdString(), getServiceAddress().toStdString(), getServicePort());
    }
    else {
        spdlog::error("[{}] Failed to connect to actuator service at address '{}' and port '{}'.", name().toStdString(), getServiceAddress().toStdString(), getServicePort());
        return ;
    }
}

void ActuatorClient::loop() {
    // Check if is disconnected from server and try to reconnect
    if(!isConnectedToServer()) {
        spdlog::warn("[{}] Disconnected from actuator service, trying to reconnect...", name().toStdString());
        bool couldRecconect = connectToServer(true);

        if(couldRecconect) {
            spdlog::info("[{}] Succesfully recconected to actuator service at address '{}' and port '{}'.", name().toStdString(), getServiceAddress().toStdString(), getServicePort());
        }
        else {
            spdlog::warn("[{}] Reconnection attempt to actuator service at address '{}' and port '{}' failed.", name().toStdString(), getServiceAddress().toStdString(), getServicePort());
            return ;
        }
    }

    // Get stub to cast getControls rpc
    grpc::ClientContext context;
    google::protobuf::Empty request;
    std::unique_ptr<grpc::ClientReader<Armorial::ControlPacket>> reader = getStub()->GetControls(&context, request);

    // Iterate over reader packets
    Armorial::ControlPacket packet;
    while(reader->Read(&packet)) {
        // Send data to actuator
        _actuator->sendData(packet);

        // Mark robot as received to further send zero packet
        _receivedCommands[packet.robotidentifier().robotcolor().isblue()][packet.robotidentifier().robotid()] = true;
    }
}

void ActuatorClient::finalization() {
    // Send zero packet to all robots that this actuator has sent commands
    for (auto &k : _receivedCommands.keys()) {
        for (auto &ki : _receivedCommands[k].keys()) {
            // Create color object
            Armorial::Color *color = new Armorial::Color;
            color->set_isblue(k);

            // Create robot identifier
            Armorial::RobotIdentifier identifier;
            identifier.set_robotid(ki);
            identifier.set_allocated_robotcolor(color);

            // Send zero data to specified robot
            _actuator->sendZeroData(identifier);
        }
    }

    spdlog::info("[{}] Disconnected from actuator service.", name().toStdString());
}
