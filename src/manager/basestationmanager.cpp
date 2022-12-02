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


#include "basestationmanager.h"

#include <spdlog/spdlog.h>

BaseStationManager::BaseStationManager(BaseStation* baseStation) {
    _baseStation = baseStation;
}

QList<Armorial::RobotStatus> BaseStationManager::getRobotFeedbacks() {
    _mutex.lockForRead();
    QList<Armorial::RobotStatus> feedbackPackets = _feedbackPackets.values();
    _mutex.unlock();

    return feedbackPackets;
}

void BaseStationManager::receiveFeedback(const Armorial::RobotStatus& robotStatus) {
    _mutex.lockForWrite();
    _feedbackPackets.insert(robotStatus.robotidentifier().robotid(), robotStatus);
    _mutex.unlock();
}

void BaseStationManager::setRobotControls(const QList<Armorial::ControlPacket> &controlPackets) {
    _mutex.lockForWrite();
    for (const auto &cp : controlPackets) {
        _controlPackets.insert(cp.robotidentifier().robotid(), cp);
    }
    _mutex.unlock();
}

void BaseStationManager::initialization() {
    // Start base station module
    _baseStation->startup();

    spdlog::info("[{}] Module started.", entityName().toStdString());
}

void BaseStationManager::loop() {
    // Lock mutex for read get the latest control packets
    _mutex.lockForRead();
    QList<Armorial::ControlPacket> controlPackets = _controlPackets.values();
    _mutex.unlock();

    // Send the packets through the base station implementation
    _baseStation->sendData(controlPackets);
}

void BaseStationManager::finalization() {
    // Send zero data and close module
    _baseStation->sendZeroData();

    spdlog::info("[{}] Module finished.", entityName().toStdString());
}
