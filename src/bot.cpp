#include "bot.h"
#include <cstdio>
#include <zlib.h>

void Bot::init(std::string _addr, unsigned short _port, std::string _name, std::string _uuid, int _protocol) {
    addr = _addr;
    port = _port;
    name = _name;
    uuid = _uuid;
    protocol = _protocol;

    using namespace boost::asio;

    ip::address address = ip::address::from_string(addr);
    socket.connect(ip::tcp::endpoint(address, port));

    loginPacket();

    io->run(); // run async_readsssss // esto me tuvo como 17 hras
}

void Bot::pushVarInt(short shortValue) {
    do {
        char byte = shortValue & 0x7F; // Obtener los 7 bits menos significativos
        shortValue >>= 7;              // Desplazar los bits restantes
        if(shortValue != 0) {          // Si quedan más bytes por codificar
            byte |= 0x80;              // Establecer el bit más significativo a 1 para indicar más bytes
        }
        sendBuff.push_back(byte); // Agregar el byte al vector
    } while(shortValue != 0);
}

void Bot::pushString(std::string str) {
    sendBuff.push_back(str.size());
    for(char c : str) sendBuff.push_back(c);
}

void Bot::pushUShort(unsigned short us) {
    sendBuff.push_back(us >> 8);
    sendBuff.push_back(us);
}

void Bot::pushUUID() {
    for(size_t i = 0; i < uuid.size(); i += 2) {
        std::string byteString = uuid.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        sendBuff.push_back(byte);
    }
}

uint32_t Bot::decodeVarInt(void) {
    uint8_t shift = 0;
    uint32_t value = 0;

    uint8_t buffByte = 0;

    do {
        buffByte = readBuff[0];
        value |= (buffByte & 0x7F) << shift;
        shift += 7;

        if(!readBuff.empty()) readBuff.erase(readBuff.begin());
    } while((buffByte & 0x80) != 0);

    return value;
}

void Bot::send() {
    sendBuff.insert(sendBuff.begin(), sendBuff.size());
    write(socket, boost::asio::buffer(sendBuff));
    sendBuff.clear();
}

void Bot::read(void) {
    packetLen = 0;
    socket.async_read_some(
    boost::asio::buffer(&packetLen, 1), std::bind(&Bot::handler, this, std::placeholders::_1, std::placeholders::_2));
}

void Bot::decodePacketLength(void) {
    uint32_t value = 0;
    uint8_t shift = 0;

    uint8_t buffByte = 0;
    do {
        buffByte = packetLen;
        value |= (buffByte & 0x7F) << shift;
        shift += 7;

        if((buffByte & 0x80) != 0)socket.read_some(boost::asio::buffer(&packetLen, 1));

    } while((buffByte & 0x80) != 0);

    packetLen = value;
}

void Bot::uncompressPacket(void) {
    long unsigned int uncompressedLen = decodeVarInt();
    std::vector<uint8_t> uncompressed(uncompressedLen);

    uncompress(uncompressed.data(), &uncompressedLen, readBuff.data(), packetLen);

    readBuff.clear();
    readBuff = uncompressed;
}

void Bot::loginPacket(void) {

    pushVarInt(0x00); // propio del protocolo
    pushVarInt(protocol);
    pushString(addr);
    pushUShort(port);
    sendBuff.push_back(0x02); // propio del protocolo

    send(); // mando el paquete;

    // segundo paquete
    pushVarInt(0x00); // propio del protocolo
    pushString(name);
    pushUUID();

    send(); // mando el paquete

    printf(INFO "send login paquet" RESET);

    // comienzo a escuchar
    read();
}

void Bot::handler(const boost::system::error_code& _err, std::size_t _len) {

    if(_err) {
        printf(ERROR "ERROR EN EL PAQUETE -> %s" RESET, _err.message().c_str());
        return;
    }

    // por si viene un VarInt
    if((packetLen & 0x80) != 0) decodePacketLength();
    
    readBuff.resize(packetLen);
    socket.read_some(boost::asio::buffer(readBuff));


    // funca? //tute del futuro, si?????????????? , si. increible.

    if(packetLen > compression_threshold) { uncompressPacket(); }

    packetID = decodeVarInt();
    id = decodeVarInt();

    printf(DEBUG "status = %u, PacketID = 0x%02X, id = 0x%02X, packetLen = 0x%02X -> %d, readBuff = %zu" RESET, status,
    packetID, id, packetLen, packetLen,readBuff.size());

    switch(status) {
        case login: loginHandler(); break;
        case config: configHandler(); break;
        case play: playHandler(); break;
    }

    read();
}


void Bot::loginHandler(void) {
    switch(packetID) {
        case 0x00:
            if(id == 0x02) {
                pushVarInt(0x00);
                sendBuff.push_back(0x03);

                send();

                status = config;
                printf(INFO "login successfully" RESET);
            }

            break;

        case 0x03: // compression packet
            compression_threshold = id;

            printf(INFO "compression_threshold = %d" RESET, compression_threshold);
            break;

        default: break;
    }
}

void Bot::configHandler(void) {
    printf(DEBUG "configHandler" RESET);

    if(packetID == 0x00 && id == 0x02) {
        pushVarInt(0x00);
        pushVarInt(0x02);
        send();

        status = play;
        printf(INFO "config finished" RESET);
    }
}

void Bot::playHandler(void) {
    if(packetID == 0x00 && id == 0x24) {
        pushVarInt(0x00);
        pushVarInt(0x15);
        sendBuff.insert(sendBuff.end(), readBuff.begin(), readBuff.end());

        // printf(DEBUG "BUFFER :" RESET);

        // for(uint8_t c : sendBuff) { printf(DEBUG "0x%02X" RESET, c); }

        send();

        printf(INFO "keep alive" RESET);
    }
}
