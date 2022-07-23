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

#ifndef SIMULATIONACTUATOR_H
#define SIMULATIONACTUATOR_H

#include <Armorial/Base/Client/Client.h>

#include <include/proto/ssl_simulation_robot_control.pb.h>
#include <src/actuators/baseactuator.h>

/*!
 * \brief The SimulationActuator class provides a interface to communicate with the simulation (grSim / simulator-cli)
 */
class SimulationActuator : public BaseActuator, public Base::UDP::Client
{
public:
    /*!
     * \brief SimulationActuator constructor.
     * \param serverAddress, serverPort The network parameters which the actuator will be connected to.
     */
    SimulationActuator(QString serverAddress, quint16 serverPort);

    /*!
     * \brief sendData Override from parent that describes how to send a given data using this actuator.
     * \param packet The given data.
     */
    void sendData(const Armorial::ControlPacket& packet);

    /*!
     * \brief sendZeroData Override from parent that sends a zero packet to a given identifier.
     * \param robotIdentifier The given identifier.
     */
    void sendZeroData(const Armorial::RobotIdentifier& robotIdentifier);
};

#endif // SIMULATIONACTUATOR_H
