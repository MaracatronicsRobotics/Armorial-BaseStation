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

#ifndef BASESTATIONSERVICE_H
#define BASESTATIONSERVICE_H

#include <Armorial/Base/Service/Service.h>

#include <include/proto/basestationservice.grpc.pb.h>

#include <src/manager/basestationmanager.h>

/*!
 * \brief The BaseStationService class
 */
class BaseStationService : public Threaded::Entity, public Base::GRPC::Service<Armorial::BaseStation::BaseStationService::Service>
{
public:
    /*!
     * \brief BaseStationService
     */
    BaseStationService(QString baseStationServiceAddress, quint16 baseStationServicePort, BaseStationManager* baseStationManager);

    /*!
     * \brief gRPC rpc implementations
     */
    grpc::Status SetControls(grpc::ServerContext* context, grpc::ServerReader<Armorial::ControlPacket>* reader, google::protobuf::Empty* response);
    grpc::Status GetRobotsStatus(grpc::ServerContext* context, const google::protobuf::Empty* request, grpc::ServerWriter<Armorial::RobotStatus>* writer);
    grpc::Status Check(grpc::ServerContext* context, const Armorial::HealthCheckRequest* request, Armorial::HealthCheckResponse* response);
    grpc::Status Watch(grpc::ServerContext* context, const Armorial::HealthCheckRequest* request, grpc::ServerWriter<Armorial::HealthCheckResponse>* writer);

private:
    /*!
     * \brief Entity inherited methods.
     */
    void initialization();
    void loop();
    void finalization();

    // Manager to receive the control packets and obtain the feedbacks
    BaseStationManager* _baseStationManager;
};

#endif // BASESTATIONSERVICE_H
