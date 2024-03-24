#include <boost/config/detail/suffix.hpp>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

// zlib para descomprimir
#include <zlib.h>


#define ERROR "\033[1;31m[E]-> " // Red
#define INFO "\033[1;32m[I]-> "
#define DEBUG "\033[1;33m[D]-> "
#define RESET "\033[0m\n"
#define BLUE_COLOR "\033[0;34m"
#define PACKET "\033[0;35m[P]->" // magenta
#define CYAN_COLOR "\033[0;36m"
#define WHITE_COLOR "\033[0;37m" // White

namespace types {

struct player_t {
    struct {
        float hp;
        uint8_t food;
        float foodSat;
    } healt;
};

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

namespace packet {

// decode vars
uint16_t decodeVarInt(std::vector<uint8_t>&);
uint64_t decodeVarLong(std::vector<uint8_t>&);
__uint128_t decodeUUID(std::vector<uint8_t>&);
float decodeFloat(std::vector<uint8_t>& buff);
double decodeDouble(std::vector<uint8_t>&);
uint8_t decodeByte(std::vector<uint8_t>& buff);
uint16_t decodeShort(std::vector<uint8_t>& buff);

// decode structs
types::entity_t decodeEntity(std::vector<uint8_t>&);
void decodeHealt(std::vector<uint8_t>&, types::player_t&);

void pushVarInt(std::vector<uint8_t>&, uint16_t);
void pushString(std::vector<uint8_t>&, std::string);
void pushUShort(std::vector<uint8_t>&, uint16_t);
void pushByte(std::vector<uint8_t>&, uint8_t);

void pushStrUUID(std::vector<uint8_t>&, std::string);

void uncompressPacket(std::vector<uint8_t>&, uint16_t);

void hexDebugPrint(std::vector<uint8_t>);
} // namespace packet
