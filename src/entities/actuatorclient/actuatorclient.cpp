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

#include <src/utils/text/text.h>

ActuatorClient::ActuatorClient(BaseActuator *actuator, Constants *constants) {
    _constants = constants;

    _actuatorServiceAddress = getConstants()->getGRPCAddress();
    _actuatorServicePort = getConstants()->getGRPCPort();
    _actuator = actuator;
}

QString ActuatorClient::name() {
    return "ActuatorClient";
}

bool ActuatorClient::isConnectedToServer() {
    return (   _channel->GetState(false) == GRPC_CHANNEL_READY
            || _channel->GetState(false) == GRPC_CHANNEL_IDLE
            || _channel->GetState(false) == GRPC_CHANNEL_CONNECTING);
}

void ActuatorClient::connectToServer() {
    QString serviceAddress = QString("%1:%2").arg(_actuatorServiceAddress).arg(_actuatorServicePort);
    _channel = grpc::CreateChannel(serviceAddress.toStdString(), grpc::InsecureChannelCredentials());
    _stub = Actuator::ActuatorService::NewStub(_channel);
}

Constants* ActuatorClient::getConstants() {
    if(_constants == nullptr) {
        std::cout << Text::yellow("[WARNING] ", true) + Text::bold("Constants with nullptr value at ActuatorClient.\n");
    }

    return _constants;
}

void ActuatorClient::initialization() {
    connectToServer();

    // Initialize QTimer for each robot
    for(int i = 0; i <= getConstants()->getQtdPlayers(); i++) {
        Timer *timer = new Timer();
        timer->start();
        _timers.insert(i, timer);
    }

    // Debug to notify connection
    std::cout << Text::blue("[ACTUATOR CLIENT] ", true) + Text::bold("Connected to gRPC in address ") + Text::green(_actuatorServiceAddress.toStdString() + ":" + std::to_string(_actuatorServicePort), true) + '\n';
}

void ActuatorClient::loop() {
    for(int i = 0; i <= getConstants()->getQtdPlayers(); i++) {
        _timers.value(i)->stop();

        if(_timers.value(i)->getMiliSeconds() >= getConstants()->getTimeToSendPacketZero()) { // constante
            _actuator->sendZeroData(i);
        }
    }

    if(!isConnectedToServer()) return ;

    grpc::ClientContext context;
    google::protobuf::Empty request;

    std::unique_ptr<grpc::ClientReader<ControlPacket>> reader = _stub->GetControls(&context, request);

    ControlPacket packet;
    while(reader->Read(&packet)) {
        _actuator->sendData(packet);
        _timers.value(packet.robotidentifier().robotid())->start();
    }
}

void ActuatorClient::finalization() {
    std::cout << Text::blue("[ACTUATOR CLIENT] ", true) + Text::bold("Disconnected from gRPC server.\n");
}
