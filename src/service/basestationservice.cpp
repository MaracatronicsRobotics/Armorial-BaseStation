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

#include "basestationservice.h"

#include <spdlog/spdlog.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

BaseStationService::BaseStationService(QString baseStationServiceAddress, quint16 baseStationServicePort, BaseStationManager* baseStationManager) : Base::GRPC::Service<Armorial::BaseStation::BaseStationService::Service>(baseStationServiceAddress, baseStationServicePort) {
    _baseStationManager = baseStationManager;
}

grpc::Status BaseStationService::SetControls(grpc::ServerContext* context, grpc::ServerReader<Armorial::ControlPacket>* reader, google::protobuf::Empty* response) {
    // Read until the connection is stabilished
    QList<Armorial::ControlPacket> controlPackets;
    Armorial::ControlPacket request;
    while(reader->Read(&request)) {
        controlPackets.push_back(request);
    }

    // Send to manager
    _baseStationManager->setRobotControls(controlPackets);

    // Return ok status
    return grpc::Status::OK;
}

grpc::Status BaseStationService::GetRobotsStatus(grpc::ServerContext* context, const google::protobuf::Empty* request, grpc::ServerWriter<Armorial::RobotStatus>* writer) {
    // Take feedbacks from manager
    QList<Armorial::RobotStatus> robotFeedbacks = _baseStationManager->getRobotFeedbacks();

    // If no status packets, finish connection.
    if(robotFeedbacks.isEmpty()) {
        return grpc::Status::CANCELLED;
    }

    // Iterate over the feedbacks and send them through the rpc call
    for (const auto& rf : robotFeedbacks) {
        writer->Write(rf);
    }

    // Return ok status
    return grpc::Status::OK;
}

grpc::Status BaseStationService::Check(grpc::ServerContext* context, const Armorial::HealthCheckRequest* request, Armorial::HealthCheckResponse* response) {
    response->set_status(response->SERVING);

    // Return ok status
    return grpc::Status::OK;
}

grpc::Status BaseStationService::Watch(grpc::ServerContext *context, const Armorial::HealthCheckRequest *request, grpc::ServerWriter<Armorial::HealthCheckResponse> *writer) {
    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented.");
}

void BaseStationService::initialization() {
    if(initializeService()) {
        spdlog::info("[{}] Initialized service at address '{}' and port '{}'.", serviceName().toStdString(), getServiceAddress().toStdString(), getServicePort());
    }
    else {
        spdlog::error("[{}] Failed to initialize the service at address '{}' and port '{}'.", serviceName().toStdString(), getServiceAddress().toStdString(), getServicePort());
        return ;
    }

    spdlog::info("[{}] Module started.", entityName().toStdString());
}

void BaseStationService::loop() {

}

void BaseStationService::finalization() {
    spdlog::info("[{}] Finished service thread.", serviceName().toStdString());
}
