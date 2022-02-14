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


#ifndef ACTUATORCLIENT_H
#define ACTUATORCLIENT_H

#include <grpc/grpc.h>

#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <QList>
#include <QTimer>
#include <src/entities/entity.h>
#include <src/actuator/baseactuator.h>
#include <proto/actuatorservice.grpc.pb.h>

class ActuatorClient : public Entity
{
public:
    ActuatorClient(QString actuatorServiceAddress, quint16 actuatorServicePort, BaseActuator *actuator);

    QString name();

protected:
    bool isConnectedToServer();
    void connectToServer();

private:
    // Entity inherited methods
    void initialization();
    void loop();
    void finalization();

    // gRPC channel related network methods and vars
    std::unique_ptr<Actuator::ActuatorService::Stub> _stub;
    std::shared_ptr<grpc::Channel> _channel;
    bool getControlPackets(QList<ControlPacket> &controlPacketList);
    QMap<int, Timer*> _timers;

    // Network
    QString _actuatorServiceAddress;
    quint16 _actuatorServicePort;
    BaseActuator *_actuator;
};

#endif // ACTUATORCLIENT_H
