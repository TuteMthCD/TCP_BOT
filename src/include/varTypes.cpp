#include "varTypes.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <vector>

uint16_t packet::decodeVarInt(std::vector<uint8_t>& buff) {
    uint8_t shift = 0;
    uint32_t value = 0;

    uint8_t buffByte = 0;

    do {
        if(buff.empty()) return 0;

        buffByte = buff[0];
        value |= (buffByte & 0x7F) << shift;
        shift += 7;

        if(!buff.empty()) buff.erase(buff.begin());
    } while((buffByte & 0x80) != 0);

    return value;
}

uint64_t packet::decodeVarLong(std::vector<uint8_t>& buff) {
    uint8_t shift = 0;
    uint64_t value = 0;

    uint8_t buffByte = 0;

    do {
        if(buff.empty()) return 0;

        buffByte = buff[0];
        value |= (static_cast<uint64_t>(buffByte & 0x7F) << shift);
        shift += 7;

        buff.erase(buff.begin());
    } while((buffByte & 0x80) != 0);

    return value;
}
double packet::decodeDouble(std::vector<uint8_t>& buff) {
    double_t value;
    uint16_t len = sizeof(value);

    std::reverse(buff.begin(), buff.begin() + len); // la doy vuelta(como a ella)
    std::memcpy(&value, buff.data(), len);
    buff.erase(buff.begin(), buff.begin() + len);

    return value;
}

__uint128_t packet::decodeUUID(std::vector<uint8_t>& buff) {
    __uint128_t UUID = 0;
    auto ite = buff.begin();

    for(int i = 0; i < 16; i++) UUID = buff[i] << (i * 8);
    buff.erase(ite, ite + 16);

    return UUID;
}

uint8_t packet::decodeByte(std::vector<uint8_t>& buff) {
    uint8_t value = buff[0];
    buff.erase(buff.begin());

    return value;
}
uint16_t packet::decodeShort(std::vector<uint8_t>& buff) {
    uint16_t value = buff[0] + (buff[1] << 8);
    auto ite = buff.begin();

    buff.erase(ite, ite + 2);
    return value;
}


/*------------------push------------------------------------------*/

void packet::pushVarInt(std::vector<uint8_t>& buff, uint16_t value) {
    do {
        char byte = value & 0x7F; // Obtener los 7 bits menos significativos
        value >>= 7;              // Desplazar los bits restantes
        if(value != 0) {          // Si quedan más bytes por codificar
            byte |= 0x80;         // Establecer el bit más significativo a 1 para indicar más bytes
        }
        buff.push_back(byte); // Agregar el byte al vector
    } while(value != 0);
}

void packet::pushString(std::vector<uint8_t>& buff, std::string str) {
    buff.push_back(str.size());
    for(char c : str) buff.push_back(c);
}

void packet::pushUShort(std::vector<uint8_t>& buff, uint16_t value) {
    buff.push_back(value >> 8);
    buff.push_back(value);
}
void packet::pushByte(std::vector<uint8_t>& buff, uint8_t value) {
    buff.push_back(value);
}

void packet::pushStrUUID(std::vector<uint8_t>& buff, std::string uuid) {
    for(size_t i = 0; i < uuid.size(); i += 2) {
        std::string byteString = uuid.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        buff.push_back(byte);
    }
}

void packet::uncompressPacket(std::vector<uint8_t>& buff, uint16_t packetLen) {
    long unsigned int uncompressedLen = packet::decodeVarInt(buff);
    std::vector<uint8_t> uncompressed(uncompressedLen);

    uncompress(uncompressed.data(), &uncompressedLen, buff.data(), packetLen);

    buff.clear();
    buff = uncompressed;
}


void packet::hexDebugPrint(std::vector<uint8_t> buff) {
    printf(DEBUG);
    for(uint8_t c : buff) printf("%02X ", c);
    printf(RESET);
}


types::entity_t packet::decodeEntity(std::vector<uint8_t>& buff) {
    printf(INFO "entity spaw" RESET);

    types::entity_t entity;

    entity.ID = packet::decodeVarInt(buff);

    entity.UUID = decodeUUID(buff);

    entity.typeID = packet::decodeVarInt(buff);

    entity.x = packet::decodeDouble(buff);
    entity.y = packet::decodeDouble(buff);
    entity.z = packet::decodeDouble(buff);

    entity.pitch = decodeByte(buff);
    entity.yaw = decodeByte(buff);
    entity.headYaw = decodeByte(buff);

    entity.data = decodeVarInt(buff);

    entity.xVel = decodeShort(buff);
    entity.yVel = decodeShort(buff);

    packet::hexDebugPrint(buff);

    entity.zVel = decodeShort(buff);

    printf(DEBUG "entity xVel -> %d" RESET, entity.zVel);

    return entity;
}
