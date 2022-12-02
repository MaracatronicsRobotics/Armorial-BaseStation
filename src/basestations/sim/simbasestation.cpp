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

#include "simbasestation.h"

#include <include/proto/ssl_simulation_robot_control.pb.h>
#include <include/proto/ssl_simulation_robot_feedback.pb.h>

#include <spdlog/spdlog.h>

SimBaseStation::SimBaseStation(QString simAddress, quint16 simControlPort) : Base::UDP::Client(simAddress, simControlPort) {
    _simAddress = simAddress;
    _simControlPort = simControlPort;
}

void SimBaseStation::startup() {
    bool connected = connectToNetwork();
    if(connected) {
        spdlog::info("[{}] Connected to simulator at address '{}' and port '{}'.", clientName().toStdString(), _simAddress.toStdString(), _simControlPort);
    }
    else {
        spdlog::warn("[{}] Could not connect to simulator at address '{}' and port '{}'.", clientName().toStdString(), _simAddress.toStdString(), _simControlPort);
    }
}

void SimBaseStation::sendData(const QList<Armorial::ControlPacket>& packets) {
    RobotControl control;
    for(const auto &cp : packets)
    {
        RobotCommand* command = control.add_robot_commands();
        MoveLocalVelocity* localVelocity = command->mutable_move_command()->mutable_local_velocity();

        command->set_id(cp.robotidentifier().robotid());

        const Armorial::Velocity linearSpeed = cp.robotvelocity();
        localVelocity->set_forward(linearSpeed.vy());
        localVelocity->set_left(-linearSpeed.vx());

        const Armorial::AngularSpeed angularSpeed = cp.robotangularspeed();
        localVelocity->set_angular(angularSpeed.vw());

        const Armorial::KickSpeed kickSpeed = cp.robotkick();
        command->set_kick_speed(kickSpeed.kickspeed());
        command->set_kick_angle(kickSpeed.kickangle());

        command->set_dribbler_speed(cp.dribbling() * 100e3);
    }

    QByteArray datagram;
    datagram.resize(control.ByteSizeLong());
    control.SerializeToArray(datagram.data(), datagram.size());

    if(!sendDatagram(datagram)) {
        spdlog::warn("[{}] Failed to sent datagram to simulator. Is simulator running?", typeid(*this).name());
    }

    checkRobotFeedbacks();
}

void SimBaseStation::sendZeroData() {
    RobotControl control;
    for(int i = 0; i < 16; i++)
    {
        RobotCommand* command = control.add_robot_commands();
        MoveLocalVelocity* localVelocity = command->mutable_move_command()->mutable_local_velocity();

        command->set_id(i);

        localVelocity->set_forward(0.0);
        localVelocity->set_left(0.0);
        localVelocity->set_angular(0.0);
        command->set_kick_speed(0.0);
        command->set_kick_angle(0.0);
        command->set_dribbler_speed(0.0);
    }

    QByteArray datagram;
    datagram.resize(control.ByteSizeLong());
    control.SerializeToArray(datagram.data(), datagram.size());

    if(!sendDatagram(datagram)) {
        spdlog::warn("[{}] Failed to sent datagram to simulator. Is simulator running?", typeid(*this).name());
    }

    checkRobotFeedbacks();
}

void SimBaseStation::checkRobotFeedbacks() {
    while(hasPendingDatagrams())
    {
        std::optional<QNetworkDatagram> datagram = receiveDatagram();

        if(datagram.has_value()) {
            RobotControlResponse response;
            response.ParseFromArray(datagram.value().data(), datagram.value().data().length());

            if(response.ByteSizeLong() > 0)
            {
                for(int i = 0; i < response.feedback_size(); ++i)
                {
                    const RobotFeedback& feedback = response.feedback(i);

                    Armorial::RobotIdentifier* robotIdentifier = new Armorial::RobotIdentifier();
                    robotIdentifier->set_robotid(feedback.id());

                    Armorial::RobotStatus robotStatus;
                    robotStatus.set_allocated_robotidentifier(robotIdentifier);
                    robotStatus.set_infrared(feedback.has_dribbler_ball_contact());

                    emit sendFeedback(robotStatus);
                }
            }
        }

    }
}
