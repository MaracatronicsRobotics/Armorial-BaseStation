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


#include "constants.h"
#include <iostream>
#include <src/utils/text/text.h>

Constants::Constants(QString fileName) {
    _fileName = fileName;

    file.setFileName(_fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());

    QJsonObject docObject = document.object();
    QVariantMap doc_map = docObject.toVariantMap();

    _GRPCAddress = doc_map["GRPCAddress"].toString();
    std::cout << Text::bold("GRPC Address: ") + Text::green(_GRPCAddress.toStdString(), true) << std::endl;

    _GRPCPort = doc_map["GRPCPort"].toInt();
    std::cout << Text::bold("GRPC Port: ") + Text::green(std::to_string(_GRPCPort), true) << std::endl;

    _SimAddress = doc_map["SimAddress"].toString();
    std::cout << Text::bold("Simulator Address: ") + Text::green(_SimAddress.toStdString(), true) << std::endl;

    _SimPort = doc_map["SimPort"].toInt();
    std::cout << Text::bold("Simulator Port: ") + Text::green(std::to_string(_SimPort), true) << std::endl;

    _teamColor = doc_map["teamColor"].toString();
    std::cout << Text::bold("Team Color: ") + Text::green(_teamColor.toStdString(), true) << std::endl;

    _qtdPlayers = doc_map["qtdPlayers"].toInt();
    std::cout << Text::bold("Number of players: ") + Text::green(std::to_string(_qtdPlayers), true) << std::endl;

    _timeToSendPacketZero = doc_map["timeToSendPacketZero"].toFloat();
    std::cout << Text::bold("Time to send packet zero: ") + Text::green(std::to_string(_timeToSendPacketZero), true) << std::endl;

}

bool Constants::isTeamBlue(){
    if (getTeamColor() == "blue"){
        return true;
    }
    return false;
}

bool Constants::isTeamYellow(){
    if (getTeamColor() == "yellow"){
        return true;
    }
    return false;
}

QString Constants::getTeamColor() const
{
    return _teamColor;
}

void Constants::setTeamColor(const QString &teamColor)
{
    _teamColor = teamColor;
}

int Constants::getQtdPlayers() const
{
    return _qtdPlayers;
}

void Constants::setQtdPlayers(int qtdPlayers)
{
    _qtdPlayers = qtdPlayers;
}

QString Constants::getGRPCAddress() const
{
    return _GRPCAddress;
}

void Constants::setGRPCAddress(const QString &GRPCAddress)
{
    _GRPCAddress = GRPCAddress;
}

quint16 Constants::getGRPCPort() const
{
    return _GRPCPort;
}

void Constants::setGRPCPort(const quint16 &GRPCPort)
{
    _GRPCPort = GRPCPort;
}

QString Constants::getSimAddress() const
{
    return _SimAddress;
}

void Constants::setSimAddress(const QString &SimAddress)
{
    _SimAddress = SimAddress;
}

quint16 Constants::getSimPort() const
{
    return _SimPort;
}

void Constants::setSimPort(const quint16 &SimPort)
{
    _SimPort = SimPort;
}

float Constants::getTimeToSendPacketZero() const
{
    return _timeToSendPacketZero;
}

void Constants::setTimeToSendPacketZero(float timeToSendPacketZero)
{
    _timeToSendPacketZero = timeToSendPacketZero;
}

