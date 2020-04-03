#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <QtCore>
#include <QThread>
#include <QUdpSocket>
#include <GEARSystem/gearsystem.hh>
#include <utils/timer.h>

#include <iostream>

// grsim protobuf
#include <include/3rd_party/grSim_Commands.pb.h>
#include <include/3rd_party/grSim_Packet.pb.h>
#include <include/3rd_party/grSim_Replacement.pb.h>

#define QT_PLAYERS 12
#define QT_TEAMS 2

class PacketManager : public QThread, public Actuator
{
    Q_OBJECT
public:
    // name for actuator in gearsystem
    PacketManager(const QString &name);

    // Network (actuator in gearsystem)
    bool connect(const QString& serverAddress, const uint16 serverPort, const QString& grSimAddress, quint16 grSimPort);
    void disconnect();
    bool isConnected() const;

    // Thread control
    void setLoopFrequency(int hz);
    bool isRunning();
    void stopRunning();
private:
    // Thread inherit
    void run();

    // Actuator inherit
    void setSpeed(quint8 teamNum, quint8 playerNum, float x, float y, float theta);
    void kick(quint8 teamNum, quint8 playerNum, float power);
    void chipKick(quint8 teamNum, quint8 playerNum, float power);
    void kickOnTouch(quint8 teamNum, quint8 playerNum, bool enable, float power);
    void chipKickOnTouch(quint8 teamNum, quint8 playerNum, bool enable, float power);
    void holdBall(quint8 teamNum, quint8 playerNum, bool enable);

    // grSim socket
    typedef struct{
        bool isYellow;
        int id;
        double v1, v2, v3, v4;
        double vx ,vy, angle;
        double kickspeedx, kickspeedz;
        bool spinner;

        bool isUpdated;
    } grs_robot;

    QUdpSocket _socket;
    grs_robot packets[QT_TEAMS][QT_PLAYERS];
    void sendPacket(grs_robot robot);

    // Thread internal info
    QMutex _mutexRunning;
    bool _running;
    float _loopTime;

    // Actuator/thread synchronization mutex
    QMutex _writeMutex;

    // Internal
    void markPlayersAsUpdated(quint8 teamNum, quint8 playerNum);
    void markPlayersAsOutdated(quint8 teamNum, quint8 playerNum);
};

#endif // PACKETMANAGER_H
