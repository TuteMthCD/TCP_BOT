#include <cstdint>
#include <string>
#include <vector>

namespace packet {

uint16_t decodeVarInt(std::vector<uint8_t>&);
uint64_t decodeVarLong(std::vector<uint8_t>&);

void pushVarInt(std::vector<uint8_t>&, uint16_t);
void pushString(std::vector<uint8_t>&, std::string);
void pushUShort(std::vector<uint8_t>&, uint16_t);
void pushByte(std::vector<uint8_t>&, uint8_t);

void pushStrUUID(std::vector<uint8_t>&, std::string);

} // namespace packet
