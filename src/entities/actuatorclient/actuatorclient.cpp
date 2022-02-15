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

ActuatorClient::ActuatorClient(QString actuatorServiceAddress, quint16 actuatorServicePort, BaseActuator *actuator, Constants *constants) {
    _actuatorServiceAddress = actuatorServiceAddress;
    _actuatorServicePort = actuatorServicePort;
    _actuator = actuator;
    _constants = constants;
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
    for(int i = 0; i < 12; i++) { // constante
        Timer *timer = new Timer();
        timer->start();
        _timers.insert(i, timer);
    }
}

void ActuatorClient::loop() {
    for(int i = 0; i < 12; i++) { // constante
        _timers.value(i)->stop();

        if(_timers.value(i)->getMiliSeconds() >= 1000.0) { // constante
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

}
