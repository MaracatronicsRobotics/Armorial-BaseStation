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

#ifndef RADIOACTUATOR_H
#define RADIOACTUATOR_H

#include <QSerialPort>
#include <src/actuators/baseactuator.h>

/*!
 * \brief The RadioActuator class provides a interface to communicate with the robots using
 * the defined communication protocol through a serial port
 */
class RadioActuator : public BaseActuator
{
public:
    /*!
     * \brief SimulationActuator constructor.
     * \param serialPort The serial port which the RadioActuator will use to send the data.
     */
    RadioActuator(QString serialPortName);

    ~RadioActuator();

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

private:
    QSerialPort *_serialPort;
    QString _serialPortName;

    /*!
     * \brief Try to open serial port with the defined name
     */
    void openSerialPort();
};

#endif // RADIOACTUATOR_H
