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

#ifndef BASE_ACTUATOR_H
#define BASE_ACTUATOR_H

#include <include/proto/messages.pb.h>

/*!
 * \brief The BaseActuator class provides a interface for the actuator implementations.
 */
class BaseActuator
{
public:
    /*!
     * \brief BaseActuator default constructor.
     */
    BaseActuator() = default;

    /*!
     * \brief ~BaseActuator virtual destructor.
     */
    virtual ~BaseActuator() = default;

    /*!
     * \brief Virtual sendData that allows the children implementation to interpret and send the control packet
     *  to the robots.
     * \param packet The control packet which will be sent.
     */
    virtual void sendData(const Armorial::ControlPacket& packet) = 0;

    /*!
     * \brief Virtual sendZeroData that allows the children implementation to send a zero-control packet
     * to the robots.
     * \param robotIdentifier The robot identifier which will receive the data.
     */
    virtual void sendZeroData(const Armorial::RobotIdentifier& robotIdentifier) = 0;
};

#endif // BASE_ACTUATOR_H
