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

#ifndef SIMBASESTATION_H
#define SIMBASESTATION_H

#include <Armorial/Base/Client/Client.h>

#include <src/basestations/basestation.h>

class SimBaseStation : public BaseStation, public Base::UDP::Client
{
public:
    SimBaseStation(QString simAddress, quint16 simControlPort);

protected:
    void sendData(const QList<Armorial::ControlPacket>& packets);
    void sendZeroData();
    void startup();

private:
    void checkRobotFeedbacks();

    // Internal variables for sim network
    QString _simAddress;
    quint16 _simControlPort;
};

#endif // SIMBASESTATION_H
