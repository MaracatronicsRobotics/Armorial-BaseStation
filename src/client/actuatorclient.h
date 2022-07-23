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

#include <Armorial/Base/Client/Client.h>
#include <Armorial/Threaded/Entity/Entity.h>

#include <src/actuators/baseactuator.h>
#include <include/proto/actuatorservice.grpc.pb.h>

/*!
 * \brief The ActuatorClient class provides a interface to connect with the actuator gRPC service, getting
 * the upcoming control packets from the coach.
 */
class ActuatorClient : public Threaded::Entity, public Base::GRPC::Client<Armorial::Actuator::ActuatorService>
{
public:
    /*!
     * \brief ActuatorClient constructor.
     * \param serviceAddress, servicePort The service address which this client will connect to.
     * \param actuator The BaseActuator implementation which will be used to send the received packets.
     */
    ActuatorClient(QString serviceAddress, quint16 servicePort, BaseActuator *actuator);

    /*!
     * \return The name of this client.
     */
    QString name();

private:
    /*!
     * \brief Override from Threaded::Entity::initialization() to initialize the client and its dependencies.
     */
    void initialization();

    /*!
     * \brief Override from Threaded::Entity::loop() to enable threaded work scope.
     */
    void loop();

    /*!
     * \brief Override from Threaded::Entity::finalization() to finalize the client and its dependencies.
     */
    void finalization();

    // Internal vars
    BaseActuator *_actuator;
    QMap<bool, QMap<quint8, bool>> _receivedCommands;
};

#endif // ACTUATORCLIENT_H
