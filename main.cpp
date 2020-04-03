#include <QCoreApplication>
#include <exithandler.h>
#include <packetmanager.h>

/* backbone (localhost?) defines */
#define BACKBONE_ADDRESS "localhost"
#define BACKBONE_PORT 0

/* grsim host defines */
#define GRSIM_ADDRESS "localhost"
#define GRSIM_PORT 20011

/* thread defines */
#define LOOP_FREQUENCY 60 // hz

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ExitHandler::setup(&a);

    PacketManager packetManager("Armorial-Actuator");
    packetManager.connect(BACKBONE_ADDRESS, BACKBONE_PORT, GRSIM_ADDRESS, GRSIM_PORT);
    packetManager.setLoopFrequency(LOOP_FREQUENCY);
    packetManager.start();

    // wait
    int retn = a.exec();

    // finish
    packetManager.stopRunning();
    packetManager.wait();

    return retn;
}
