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


#ifndef BASESTATIONMANAGER_H
#define BASESTATIONMANAGER_H

#include <QMap>
#include <QReadWriteLock>

#include <Armorial/Threaded/Entity/Entity.h>
#include <Armorial/Common/Enums/Color/Color.h>

#include <include/proto/messages.pb.h>

#include <src/basestations/basestation.h>

class BaseStationManager : public Threaded::Entity
{
    Q_OBJECT
public:
    /*!
     * \brief BaseStationManager
     */
    BaseStationManager(BaseStation* baseStation);

    /*!
     * \brief BaseStationManager default destructor.
     */
    ~BaseStationManager() = default;

    /*!
     * \brief getRobotFeedbacks
     * \return
     */
    QList<Armorial::RobotStatus> getRobotFeedbacks();

    /*!
     * \brief setRobotControls
     * \param controlPackets
     */
    void setRobotControls(const QList<Armorial::ControlPacket>& controlPackets);

private:
    /*!
     * \brief Entity inherited methods.
     */
    void initialization();
    void loop();
    void finalization();

    // Internal variables for packets control
    QMap<quint8, Armorial::ControlPacket> _controlPackets;
    QMap<quint8, Armorial::RobotStatus> _feedbackPackets;

    // BaseStation implementation
    BaseStation* _baseStation;

    // QReadWriteLock for management
    QReadWriteLock _mutex;

public slots:
    void receiveFeedback(const Armorial::RobotStatus& robotStatus);
};

#endif // BASESTATIONMANAGER_H
