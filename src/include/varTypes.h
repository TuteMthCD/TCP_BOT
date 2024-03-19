#include <boost/config/detail/suffix.hpp>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

// zlib para descomprimir
#include <zlib.h>

namespace packet {

uint16_t decodeVarInt(std::vector<uint8_t>&);
uint64_t decodeVarLong(std::vector<uint8_t>&);

void pushVarInt(std::vector<uint8_t>&, uint16_t);
void pushString(std::vector<uint8_t>&, std::string);
void pushUShort(std::vector<uint8_t>&, uint16_t);
void pushByte(std::vector<uint8_t>&, uint8_t);

void pushStrUUID(std::vector<uint8_t>&, std::string);

void uncompressPacket(std::vector<uint8_t>&, uint16_t);

} // namespace packet


namespace types {

#pragma pack(push, 1) // para crear un struct de 9 bytes
struct player_t {
    struct {
        float foodSat;
        uint8_t food;
        float hp;
    } healt;
};
#pragma pack(pop)

struct tickTime_t {
    long timeDay = 0;
    long worldAge = 0;
};

struct entity_t {
    
    uint16_t ID;
    __uint128_t UUID;
    uint16_t typeID;
    
    double_t x;
    double_t y;
    double_t z;

    uint8_t pitch;
    uint8_t yaw;
    uint8_t headYaw;
    uint16_t data;

    uint16_t xVel;
    uint16_t yVel;
    uint16_t zVel;
};

} // namespace types
