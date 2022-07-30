#include "radioactuator.h"

#include <bitset>
#include <spdlog/spdlog.h>

#include <Armorial/Common/Packet/Packet.h>
#include <Armorial/Libs/nameof/include/nameof.hpp>

RadioActuator::RadioActuator(QString serialPortName) {
    _serialPort = nullptr;
    _serialPortName = serialPortName;
}

RadioActuator::~RadioActuator() {
    if(_serialPort != nullptr) {
        _serialPort->close();
    }

    spdlog::info("Closed serial port.");
}

static uint16_t const table_byte[] = {
    0x0000, 0xc867, 0x58a9, 0x90ce, 0xb152, 0x7935, 0xe9fb, 0x219c, 0xaac3, 0x62a4,
    0xf26a, 0x3a0d, 0x1b91, 0xd3f6, 0x4338, 0x8b5f, 0x9de1, 0x5586, 0xc548, 0x0d2f,
    0x2cb3, 0xe4d4, 0x741a, 0xbc7d, 0x3722, 0xff45, 0x6f8b, 0xa7ec, 0x8670, 0x4e17,
    0xded9, 0x16be, 0xf3a5, 0x3bc2, 0xab0c, 0x636b, 0x42f7, 0x8a90, 0x1a5e, 0xd239,
    0x5966, 0x9101, 0x01cf, 0xc9a8, 0xe834, 0x2053, 0xb09d, 0x78fa, 0x6e44, 0xa623,
    0x36ed, 0xfe8a, 0xdf16, 0x1771, 0x87bf, 0x4fd8, 0xc487, 0x0ce0, 0x9c2e, 0x5449,
    0x75d5, 0xbdb2, 0x2d7c, 0xe51b, 0x2f2d, 0xe74a, 0x7784, 0xbfe3, 0x9e7f, 0x5618,
    0xc6d6, 0x0eb1, 0x85ee, 0x4d89, 0xdd47, 0x1520, 0x34bc, 0xfcdb, 0x6c15, 0xa472,
    0xb2cc, 0x7aab, 0xea65, 0x2202, 0x039e, 0xcbf9, 0x5b37, 0x9350, 0x180f, 0xd068,
    0x40a6, 0x88c1, 0xa95d, 0x613a, 0xf1f4, 0x3993, 0xdc88, 0x14ef, 0x8421, 0x4c46,
    0x6dda, 0xa5bd, 0x3573, 0xfd14, 0x764b, 0xbe2c, 0x2ee2, 0xe685, 0xc719, 0x0f7e,
    0x9fb0, 0x57d7, 0x4169, 0x890e, 0x19c0, 0xd1a7, 0xf03b, 0x385c, 0xa892, 0x60f5,
    0xebaa, 0x23cd, 0xb303, 0x7b64, 0x5af8, 0x929f, 0x0251, 0xca36, 0x5e5a, 0x963d,
    0x06f3, 0xce94, 0xef08, 0x276f, 0xb7a1, 0x7fc6, 0xf499, 0x3cfe, 0xac30, 0x6457,
    0x45cb, 0x8dac, 0x1d62, 0xd505, 0xc3bb, 0x0bdc, 0x9b12, 0x5375, 0x72e9, 0xba8e,
    0x2a40, 0xe227, 0x6978, 0xa11f, 0x31d1, 0xf9b6, 0xd82a, 0x104d, 0x8083, 0x48e4,
    0xadff, 0x6598, 0xf556, 0x3d31, 0x1cad, 0xd4ca, 0x4404, 0x8c63, 0x073c, 0xcf5b,
    0x5f95, 0x97f2, 0xb66e, 0x7e09, 0xeec7, 0x26a0, 0x301e, 0xf879, 0x68b7, 0xa0d0,
    0x814c, 0x492b, 0xd9e5, 0x1182, 0x9add, 0x52ba, 0xc274, 0x0a13, 0x2b8f, 0xe3e8,
    0x7326, 0xbb41, 0x7177, 0xb910, 0x29de, 0xe1b9, 0xc025, 0x0842, 0x988c, 0x50eb,
    0xdbb4, 0x13d3, 0x831d, 0x4b7a, 0x6ae6, 0xa281, 0x324f, 0xfa28, 0xec96, 0x24f1,
    0xb43f, 0x7c58, 0x5dc4, 0x95a3, 0x056d, 0xcd0a, 0x4655, 0x8e32, 0x1efc, 0xd69b,
    0xf707, 0x3f60, 0xafae, 0x67c9, 0x82d2, 0x4ab5, 0xda7b, 0x121c, 0x3380, 0xfbe7,
    0x6b29, 0xa34e, 0x2811, 0xe076, 0x70b8, 0xb8df, 0x9943, 0x5124, 0xc1ea, 0x098d,
    0x1f33, 0xd754, 0x479a, 0x8ffd, 0xae61, 0x6606, 0xf6c8, 0x3eaf, 0xb5f0, 0x7d97,
    0xed59, 0x253e, 0x04a2, 0xccc5, 0x5c0b, 0x946c
};

uint16_t crc16cdma2000_byte(uint16_t crc, void const *mem, size_t len) {
    unsigned char const *data = static_cast<unsigned char const*>(mem);
    if (data == NULL)
        return 0xffff;
    for (size_t i = 0; i < len; i++) {
        crc = (crc << 8) ^
              table_byte[((crc >> 8) ^ data[i]) & 0xff];
    }
    return crc;
}

void RadioActuator::openSerialPort() {
    // Create and setup serial port
    _serialPort = new QSerialPort();
    _serialPort->setPortName(_serialPortName);
    _serialPort->setBaudRate(115200);
    _serialPort->setFlowControl(QSerialPort::NoFlowControl);
    _serialPort->setDataBits(QSerialPort::Data8);
    _serialPort->setParity(QSerialPort::NoParity);
    _serialPort->setStopBits(QSerialPort::OneStop);

    // Open serial port to write
    if (!_serialPort->open(QIODevice::WriteOnly)) {
        spdlog::error("[{}] Could not open the serial '{}' due to error '{}'.", NAMEOF_TYPE_RTTI(*this).data(), _serialPort->portName().toStdString(), _serialPort->errorString().toStdString());
        exit(-1);
    }
    else {
        spdlog::info("[{}] Connected to serial '{}'.", NAMEOF_TYPE_RTTI(*this).data(), _serialPort->portName().toStdString());
    }
}

void RadioActuator::sendData(const Armorial::ControlPacket &packet) {
    // Check if serial is nullptr and initialize if necessary
    if(_serialPort == nullptr) {
        openSerialPort();
    }

    uint8_t *packetVSS = new uint8_t[6];

    // Packet is formed by a 6-byte array:

    // Process teamId - robotId into a single uint8_t (1 byte)
    {
        // As the protocol receives (robotId [4bits])(teamId [4bits]), we just
        // need to create a new uint8_t that receives the robotId shifted 4 bits
        // to the right and is added to the teamId.
        int teamId = (packet.robotidentifier().robotcolor().isblue());
        uint8_t address = (teamId << 4) + packet.robotidentifier().robotid();
        packetVSS[0] = (address);
    }

    // Process wheel right speed (1 byte)
    {
        // As the wheel right speed is passed as a single byte, we can just take the value
        // stored in wheelRightSpeed variable and pass it to the QByteArray object
        packetVSS[1] = abs(packet.w1());
    }


    // Process wheel left speed (1 byte)
    {
        // As the wheel left speed is passed as a single byte, we can just take the value
        // stored in wheelLeftSpeed variable and pass it to the QByteArray object
        packetVSS[2] = abs(packet.w2());
    }

    // Process action flags (enableDribbling)
    {
        std::bitset<8> actionFlags;
        actionFlags[0] = 0;
        actionFlags[1] = packet.w1() > 0;
        actionFlags[2] = packet.w2() > 0;

        packetVSS[3] = (actionFlags.to_ulong());
    }

    // Process CRC
    {
        // Calculate CRC for the 4 first bytes of the packet (data itself)
        uint16_t crc = crc16cdma2000_byte(0xFFFF, (void*) packetVSS, 4);

        // Split two-byte crc into two individual bytes
        uint8_t crc_split_1 = (crc & 0xFF00) >> 8;
        uint8_t crc_split_2 = crc;

        // Set bytes in the packet
        packetVSS[4] = crc_split_1;
        packetVSS[5] = crc_split_2;
    }

    uint8_t packet_fake[6];
    for(int i = 0; i < 6; i++) {
        packet_fake[i] = packetVSS[i];
        std::cout << int(packet_fake[i]) << " ";
    }
    std::cout << std::endl;

    // Send through the serial
    _serialPort->write((char*) packet_fake, sizeof(packet_fake));
    if(!_serialPort->waitForBytesWritten()) {
        spdlog::info("[{}] Failed to wait for write in serial port '{}'.", NAMEOF_TYPE_RTTI(*this).data(), _serialPort->portName().toStdString());
    }
}

void RadioActuator::sendZeroData(const Armorial::RobotIdentifier &robotIdentifier) {
    // Check if serial is nullptr and initialize if necessary
    if(_serialPort == nullptr) {
        openSerialPort();
    }

    uint8_t *packetVSS = new uint8_t[6];

    // Packet is formed by a 6-byte array:

    // Process teamId - robotId into a single uint8_t (1 byte)
    {
        // As the protocol receives (robotId [4bits])(teamId [4bits]), we just
        // need to create a new uint8_t that receives the robotId shifted 4 bits
        // to the right and is added to the teamId.
        int teamId = (robotIdentifier.robotcolor().isblue());
        uint8_t address = (teamId << 4) + robotIdentifier.robotid();
        packetVSS[0] = (address);
    }

    // Process wheel right speed (1 byte)
    {
        // As the wheel right speed is passed as a single byte, we can just take the value
        // stored in wheelRightSpeed variable and pass it to the QByteArray object
        packetVSS[1] = 0;
    }


    // Process wheel left speed (1 byte)
    {
        // As the wheel left speed is passed as a single byte, we can just take the value
        // stored in wheelLeftSpeed variable and pass it to the QByteArray object
        packetVSS[2] = 0;
    }

    // Process action flags (enableDribbling)
    {
        std::bitset<8> actionFlags;
        actionFlags[0] = 0;
        actionFlags[1] = 0;
        actionFlags[2] = 0;

        packetVSS[3] = (actionFlags.to_ulong());
    }

    // Process CRC
    {
        // Calculate CRC for the 4 first bytes of the packet (data itself)
        uint16_t crc = crc16cdma2000_byte(0xFFFF, (void*) packetVSS, 4);

        // Split two-byte crc into two individual bytes
        uint8_t crc_split_1 = (crc & 0xFF00) >> 8;
        uint8_t crc_split_2 = crc;

        // Set bytes in the packet
        packetVSS[4] = crc_split_1;
        packetVSS[5] = crc_split_2;
    }

    uint8_t packet_fake[6];
    for(int i = 0; i < 6; i++) {
        packet_fake[i] = packetVSS[i];
    }

    // Send through the serial
    _serialPort->write((char*) packet_fake, sizeof(packet_fake));
    if(!_serialPort->waitForBytesWritten()) {
        spdlog::info("[{}] Failed to wait for write in serial port '{}'.", NAMEOF_TYPE_RTTI(*this).data(), _serialPort->portName().toStdString());
    }
}
