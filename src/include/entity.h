#include <cstdint>
#include <vector>

namespace entity {

struct entity_t {
    uint16_t id;
    uint8_t uuid[8];
    uint16_t varInt;
    double x;
    double y;
    double z;
    uint8_t pitch;
    uint8_t yaw;
    uint8_t head_yaw;
    uint16_t data;
    uint16_t vel_x;
    uint16_t vel_y;
    uint16_t vel_z;
};

void spawn(std::vector<uint8_t>);
void remove(std::vector<uint8_t>);
void updatePos(void);
void updateRot(void);
void updatePosAndRot(void);
void event(void);
void setVel(void);
void teleport(void);
} // namespace entity
