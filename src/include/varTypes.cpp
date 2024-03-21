#include "varTypes.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

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
    for(uint8_t c : buff) printf("0x%02X ,", c);
    printf(RESET);
}


types::entity_t packet::decodeEntity(std::vector<uint8_t>& buff) {
    printf(INFO "entity spaw" RESET);

    types::entity_t entity;

    entity.ID = packet::decodeVarInt(buff);

    for(int i = 0; i < 16; i++) entity.UUID = buff[i] << (i * 8);
    buff.erase(buff.begin(), buff.begin() + 16);

    entity.typeID = packet::decodeVarInt(buff);

    entity.x = packet::decodeDouble(buff);
    entity.y = packet::decodeDouble(buff);
    entity.z = packet::decodeDouble(buff);

    // std::memcpy(&entity.yaw, &buff, sizeof(entity.yaw) * 3);
    //
    // buff.erase(buff.begin(), buff.begin() + 3);
    //
    // entity.data = packet::decodeVarInt(buff);
    //
    printf(INFO "packetsize -> %zu" RESET, buff.size());

    printf(DEBUG "ID -> 0x%02X , UUID -> 0x%lX%lX , typeID -> 0x%02X , pos-> %f,%f,%f" RESET, entity.ID,
    (uint64_t)(entity.UUID >> 64), (uint64_t)entity.UUID, entity.typeID, entity.x , entity.y , entity.z);

    // packet::hexDebugPrint(buff);
    //
    return entity;
}
