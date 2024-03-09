#include "bot.h"
#include <cstdio>
#include <zlib.h>

void Bot::init(std::string _addr, unsigned short _port, std::string _name, std::string _uuid, int _protocol) {
    addr = _addr;
    port = _port;
    name = _name;
    uuid = _uuid;
    protocol = _protocol;

    boost::asio::ip::address address = boost::asio::ip::address::from_string(addr);
    socket.connect(boost::asio::ip::tcp::endpoint(address, port));

    loginPacket();

    handler();
    // thread = std::thread(&Bot::handler, this);
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

uint16_t Bot::decodeVarInt(void) {
    uint8_t shift = 0;
    uint16_t value = 0;

    uint8_t buffByte = 0;

    do {
        buffByte = readBuff[0];
        value |= (buffByte & 0x7F) << shift;
        shift += 7;

        if(!readBuff.empty()) readBuff.erase(readBuff.begin());
    } while((buffByte & 0x80) != 0);

    return value;
}

bool Bot::read(void) {

    mtxSocket.lock();

    uint16_t length_buffer = socket.available();

    // Read the rest based on the determined length
    if(length_buffer) {

        readBuff.clear();
        readBuff.resize(length_buffer);

        boost::asio::read(socket, boost::asio::buffer(readBuff));

        // for(unsigned char c : buff) printf("0x%02X ", c);
        // printf("\n");

        mtxSocket.unlock();

        return true;
    }

    mtxSocket.unlock();

    return false;
}

void Bot::send() {

    mtxSocket.lock();

    sendBuff.insert(sendBuff.begin(), sendBuff.size());
    write(socket, boost::asio::buffer(sendBuff));
    sendBuff.clear();

    mtxSocket.unlock();
}

void Bot::uncompressPacket(void) {
    long unsigned int uncompressed_len = decodeVarInt();
    std::vector<uint8_t> uncompressed(uncompressed_len);

    long unsigned int readBuff_len = readBuff.size();

    uncompress(uncompressed.data(), &uncompressed_len, readBuff.data(), readBuff_len);

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
}

void Bot::handler(void) {
    printf(INFO "thread init" RESET);
    // funca? //tute del futuro, si?????????????? , si. increible.
    while(!th_stop) {
        if(read()) {
            uint16_t len = decodeVarInt();

            if(len > compression_threshold) {
                printf(INFO "uncompressPacket len = %d" RESET, len);
                uncompressPacket();
            }

            switch(status) {
                case login: loginHandler(); break;
                case config: configHandler(); break;
                case play: playHandler(); break;
            }
        }
    }
}


void Bot::loginHandler(void) {
    uint16_t id = decodeVarInt();
    switch(id) {
        case 0x00:
            if(decodeVarInt() == 0x02) {
                pushVarInt(0x00);
                sendBuff.push_back(0x03);

                printf(INFO "login successfully" RESET);

                send();
                status = config;
            }

            break;

        case 0x03: // compression packet
            compression_threshold = decodeVarInt();

            printf(INFO "compression_threshold = %d" RESET, compression_threshold);
            break;

        default:
            printf(ERROR "login-id = 0x%02X" RESET, id);

            for(unsigned char c : readBuff) printf("0x%02X ", c);
            printf("\n");

            break;
    }
}

void Bot::configHandler(void) {
    uint16_t id = decodeVarInt();
    switch(id) {
        default:
            printf(DEBUG "config-id = 0x%02X\n", id);

            printf(PACKET "HEX: ");
            for(unsigned char c : readBuff) printf("0x%02X ", c);
            printf(RESET);

            // printf(PACKET "ASCI: ");
            // for(unsigned char c : readBuff) printf("%c", c);
            // printf(RESET);

            break;
    }
}

void Bot::playHandler(void) {
    printf("playHandler \n");
}
