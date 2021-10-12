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

#include "packetmanager.h"
#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QDataStream>

PacketManager::PacketManager(const QString &name) : Actuator(name)
{
    for(int x = 0; x < QT_TEAMS; x++){
        for(int y = 0; y < QT_PLAYERS; y++){
            packets[x][y].id = y;
            packets[x][y].isYellow = x ? false : true;
            packets[x][y].vx = 0.0;
            packets[x][y].vy = 0.0;
            packets[x][y].angle = 0.0;
            packets[x][y].spinner = false;
            packets[x][y].kickspeedx = 0.0;
            packets[x][y].kickspeedz = 0.0;

            packets[x][y].isUpdated = false;
        }
    }

    _running = true;
    _loopTime = 1000;
}

void PacketManager::sendPacket(grs_robot robot){
    grSim_Packet packet;
    packet.mutable_commands()->set_isteamyellow(robot.isYellow);
    packet.mutable_commands()->set_timestamp(0.0);

    grSim_Robot_Command *command = packet.mutable_commands()->add_robot_commands();

    command->set_id(robot.id);

    command->set_wheelsspeed(false);
    command->set_wheel1(robot.v1);
    command->set_wheel2(robot.v2);
    command->set_wheel3(robot.v3);
    command->set_wheel4(robot.v4);
/*
    command->set_veltangent(robot.vy);
    command->set_velnormal(robot.vx);
    command->set_velangular(robot.angle);
*/

    command->set_veltangent(robot.vx);
    command->set_velnormal(robot.vy);
    command->set_velangular(robot.angle);

    command->set_kickspeedx(robot.kickspeedx);
    command->set_kickspeedz(robot.kickspeedz);
    command->set_spinner(robot.spinner);

    std::string s;
    packet.SerializeToString(&s);

    QByteArray arr;
    arr.resize(packet.ByteSizeLong());
    packet.SerializeToArray(arr.data(), arr.size());
    QNetworkDatagram datagram(arr, _addrSimulator, _serverPort);

    if(_socket.writeDatagram(datagram) == -1) {
        std::cout << "[Armorial-Actuator] Failed to write to socket: " << _socket.errorString().toStdString() << std::endl;
    }
}

void PacketManager::run(){
    Timer loopController;
    while(isRunning()){
        loopController.start();
        if(isConnected()){
            for(int x = 0; x < QT_TEAMS; x++){
                for(int y = 0; y < QT_PLAYERS; y++){
                    _writeMutex.lock();
                    if(packets[x][y].isUpdated){
                        sendPacket(packets[x][y]);
                        markPlayersAsOutdated(x, y);
                    }
                    _writeMutex.unlock();
                }
            }
        }
        loopController.stop();

        // loop time control
        if(isRunning()){
            long rest = _loopTime - loopController.timemsec();
            if(rest >= 0){
                msleep(rest);
            }
            else{
                std::cout << "[TIMER OVEREXTENDED] " << " Armorial-Actuator for " <<  -rest  << " ms.\n";
            }
        }
    }
}

void PacketManager::setLoopFrequency(int hz) {
    if(hz != 0) _loopTime = 1000/hz;
}

void PacketManager::markPlayersAsUpdated(quint8 teamNum, quint8 playerNum) {
    packets[teamNum][playerNum].isUpdated = true;
}

void PacketManager::markPlayersAsOutdated(quint8 teamNum, quint8 playerNum) {
    packets[teamNum][playerNum].isUpdated = false;
}

bool PacketManager::isRunning() {
    bool result;

    _mutexRunning.lock();
    result = _running;
    _mutexRunning.unlock();

    return result;
}

void PacketManager::stopRunning() {
    _mutexRunning.lock();
    _running = false;
    _mutexRunning.unlock();
}

bool PacketManager::connect(const QString& serverAddress, const uint16 serverPort, const QString& grSimAddress, const quint16 grSimPort, QString networkInterface) {
    // Connects to WRBackbone
    if(!Actuator::connect(serverAddress, serverPort)) {
        std::cerr << ">> [Armorial-Actuator] Failed to connect to WRBackbone server!" << std::endl;
        return false;
    }

    // Connects to grSim command listener
    if(_socket.isOpen())
        _socket.close();


    const QNetworkInterface iface = QNetworkInterface::interfaceFromName(networkInterface);
    const QNetworkAddressEntry addrEntry = iface.addressEntries().constFirst();
    _addrSimulator = addrEntry.broadcast();


    //_socket.connectToHost(grSimAddress, grSimPort, QIODevice::WriteOnly, QAbstractSocket::IPv4Protocol);

//    if(!(_socket.bind(QHostAddress::AnyIPv4, grSimPort, QUdpSocket::ShareAddress) &&
//            _socket.joinMulticastGroup(QHostAddress("224.5.23.2"), QNetworkInterface::interfaceFromName(networkInterface)))) {
//        std::cout << "[ACTUATOR] Error while binding socket.!\n";
//        return false;
//    }

    std::cout << "[Armorial-Actuator] Connected!" << std::endl;

    _grsimAddress = grSimAddress;
    _grsimPort = grSimPort;
    _networkInterface = networkInterface;

    return true;
}
void PacketManager::disconnect() {
    // Disconnect from WRBackbone
    Actuator::disconnect();

    // Close grSim socket
    if(_socket.isOpen()){
        _socket.close();
    }
}

bool PacketManager::isConnected() const {
    return (_socket.isOpen() && Actuator::isConnected());
}

void PacketManager::setSpeed(quint8 teamNum, quint8 playerNum, float x, float y, float theta) {
    // Save values
    _writeMutex.lock();

    //packets[teamNum][playerNum].vx    = -x;
    packets[teamNum][playerNum].vx    = x;
    packets[teamNum][playerNum].vy    = y;
    packets[teamNum][playerNum].angle = theta;
    markPlayersAsUpdated(teamNum, playerNum);
    _writeMutex.unlock();
}

void PacketManager::kick(quint8 teamNum, quint8 playerNum, float power) {
    // Save values
    _writeMutex.lock();
    packets[teamNum][playerNum].kickspeedx = power;
    packets[teamNum][playerNum].kickspeedz = 0.0;
    markPlayersAsUpdated(teamNum, playerNum);
    _writeMutex.unlock();
}

void PacketManager::chipKick(quint8 teamNum, quint8 playerNum, float power) {
    // Save values
    _writeMutex.lock();
    packets[teamNum][playerNum].kickspeedx = power * cos(0.42262189947);
    packets[teamNum][playerNum].kickspeedz = power * sin(0.42262189947);
    markPlayersAsUpdated(teamNum, playerNum);
    _writeMutex.unlock();
}

void PacketManager::kickOnTouch(quint8 teamNum, quint8 playerNum, bool enable, float power) {
    // Save values
    _writeMutex.lock();

    if(enable)
        packets[teamNum][playerNum].kickspeedx = power;
    else
        packets[teamNum][playerNum].kickspeedx = 0.0;

    packets[teamNum][playerNum].kickspeedz = 0.0;
    markPlayersAsUpdated(teamNum, playerNum);
    _writeMutex.unlock();
}

void PacketManager::chipKickOnTouch(quint8 teamNum, quint8 playerNum, bool enable, float power) {
    // Save values
    _writeMutex.lock();

    if (enable) {
        packets[teamNum][playerNum].kickspeedx = power*cos(0.785398);
        packets[teamNum][playerNum].kickspeedz = power*sin(0.785398);
    } else {
        packets[teamNum][playerNum].kickspeedx = 0.0;
        packets[teamNum][playerNum].kickspeedz = 0.0;
    }

    markPlayersAsUpdated(teamNum, playerNum);
    _writeMutex.unlock();
}

void PacketManager::holdBall(quint8 teamNum, quint8 playerNum, bool enable) {
    // Save values
    _writeMutex.lock();
    packets[teamNum][playerNum].spinner = enable;
    markPlayersAsUpdated(teamNum, playerNum);
    _writeMutex.unlock();
}
