#include "bot.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>

void Bot::init(std::string _addr, unsigned short _port, std::string _name, std::string _uuid, int _protocol) {
    addr = _addr;
    port = _port;
    name = _name;
    uuid = _uuid;
    protocol = _protocol;
}

bool Bot::getConnectedStatus(void) {
    return connected;
}

float Bot::getHealt(void) {
    return player.healt.hp;
}
void Bot::connect(void) {
    if(!connected) {
        connected = true;
        th = std::thread(&Bot::run, this);
    }
}

void Bot::disconnect(void) {
    if(connected) {
        connected = false;
        socket.cancel();
        socket.close();
        io->stop();
        io->reset();
        th.join();

        status = login;
    }
}

void Bot::run(void) {
    using namespace boost::asio;

    ip::address address = ip::address::from_string(addr);
    socket.connect(ip::tcp::endpoint(address, port));

    loginPacket();
    io->run(); // run async_readsssss // esto me tuvo como 17 hras
}


void Bot::send() {
    sendBuff.insert(sendBuff.begin(), sendBuff.size());
    write(socket, boost::asio::buffer(sendBuff));
    sendBuff.clear();
}

void Bot::read(void) {
    if(connected) {
        packetLen = 0;
        socket.async_read_some(boost::asio::buffer(&packetLen, 1), std::bind(&Bot::handler, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void Bot::decodePacketLength(void) {
    uint32_t value = 0;
    uint8_t shift = 0;

    uint8_t buffByte = 0;
    do {
        buffByte = packetLen;
        value |= (buffByte & 0x7F) << shift;
        shift += 7;

        if((buffByte & 0x80) != 0) socket.read_some(boost::asio::buffer(&packetLen, 1));

    } while((buffByte & 0x80) != 0);

    packetLen = value;
}


void Bot::loginPacket(void) {

    packet::pushVarInt(sendBuff, 0x00); // propio del protocolo
    packet::pushVarInt(sendBuff, protocol);
    packet::pushString(sendBuff, addr);
    packet::pushUShort(sendBuff, port);
    packet::pushByte(sendBuff, 0x02); // propio del protocolo

    send(); // mando el paquete;

    // segundo paquete
    packet::pushVarInt(sendBuff, 0x00); // propio del protocolo
    packet::pushString(sendBuff, name);
    packet::pushStrUUID(sendBuff, uuid);

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

    if(packetLen > compression_threshold) { packet::uncompressPacket(readBuff, packetLen); }

    packetID = packet::decodeVarInt(readBuff);

    switch(status) {
    case login: loginHandler(); break;
    case config: configHandler(); break;
    case play: playHandler(); break;
    }

    read();
}


void Bot::loginHandler(void) {

    id = packet::decodeVarInt(readBuff);

    switch(packetID) {
    case 0x00:
        if(id == 0x02) {
            packet::pushVarInt(sendBuff, 0x00);
            packet::pushByte(sendBuff, 0x03);

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
    // printf(DEBUG "configHandler" RESET);

    id = packet::decodeVarInt(readBuff);

    if(packetID == 0x00 && id == 0x02) {

        // ack config
        packet::pushVarInt(sendBuff, 0x00);
        packet::pushVarInt(sendBuff, 0x02);
        send();

        status = play;
        printf(INFO "config finished" RESET);
    }
    if(packetID == 0x00 && id != 0x02) {
        // conf packet
        packet::pushVarInt(sendBuff, 0x00);
        packet::pushVarInt(sendBuff, 0x00);

        packet::pushString(sendBuff, "es"); // idioma
        packet::pushByte(sendBuff, 0x02);   // render distance
        packet::pushVarInt(sendBuff, 0x00); // chat mode
        packet::pushByte(sendBuff, 0x01);   // chat colors???
        packet::pushByte(sendBuff, 0x7F);   // skinsparts
        packet::pushVarInt(sendBuff, 0x01); // main hand -> right
        packet::pushByte(sendBuff, 0x00);   // text filter

        packet::pushByte(sendBuff, 0x01); // modo normal
        // sendBuff.push_back(0x00); //modo fantasma

        send();
    }
    if(packetID == 0x01 && id == 0x01) printf(ERROR "Encrypt req" RESET);
}

void Bot::playHandler(void) {
    if(packetID == 0x00) {

        id = packet::decodeVarInt(readBuff);

        switch(id) {
        case 0x24: // keep alive;
            packet::pushVarInt(sendBuff, 0x00);
            packet::pushVarInt(sendBuff, 0x15);
            sendBuff.insert(sendBuff.end(), readBuff.begin(), readBuff.end());
            send();
            // printf(INFO "keep alive" RESET);
            break;
        case 0x62: // ticktime

            std::reverse(readBuff.begin(), readBuff.end());            // doy vuelta la pila, importarnte.
            std::memcpy(&tickTime, readBuff.data(), 2 * sizeof(long)); // copio vector a stuct

            break;


        case 0x09:
            break; // block update.

        /*Player*/
        case 0x29: printf(DEBUG "Player info" RESET); break;        // player info.
        case 0x36: printf(DEBUG "Player habilities" RESET); break;  // player habilities.
        case 0x51: printf(DEBUG "Player slot" RESET); break;        // slot selected.
        case 0x3F: printf(DEBUG "Player recipe book" RESET); break; // recipe book.
        case 0x73: printf(DEBUG "Update recipes" RESET); break;     // recipes update
        case 0x13: printf(DEBUG "Player inventory" RESET); break;   // inventory container content.
        case 0x6C: printf(DEBUG "Pickup item" RESET); break;        // Pickup item.
        case 0x3E: syncPlayerPos(); break;                          // player position.

        case 0x39: printf(INFO "ENTER COMBAT" RESET); break; // enter in combat.

        case 0x19: printf(DEBUG "Damage event" RESET); break; // damage event.
        case 0x22: printf(DEBUG "Hurt (auch)" RESET); break;  // hurt animation. (auch)
        case 0x5B:                                            // set healt

            player.healt = {
                .hp = packet::decodeFloat(readBuff),
                .food = packet::decodeVarInt(readBuff),
                .foodSat = packet::decodeFloat(readBuff),
            };
            printf(INFO "hp = %f, food = %d, foodSat = %f" RESET, player.healt.hp, player.healt.food, player.healt.foodSat);

            if(player.healt.hp <= 0) { // auto-revive
                packet::pushVarInt(sendBuff, 0x00);
                packet::pushVarInt(sendBuff, 0x08);
                packet::pushVarInt(sendBuff, 0x00);
                send();
            }

            break;
        case 0x5A: printf(DEBUG "Set xp" RESET); break; // set xp

        case 0x38:
            printf(INFO "END COMBAT" RESET);
            break; // end combat.

        /*entities*/
        case 0x01: spawnEntity(); break; // spawn entity.

        case 0x02: break; // spawn exp orb
        case 0x03: break; // entity animation.
        case 0x56:
            printf(DEBUG "Entity metadata" RESET);
            printf(DEBUG "EntityID->0x%02X" RESET, packet::decodeVarInt(readBuff));
            packet::hexDebugPrint(readBuff);
            break;                          // entity metadata.
        case 0x59: break;                   // set equipment
        case 0x71: break;                   // entity set atributes.
        case 0x40: removeEntities(); break; // remove entity.
        case 0x1D: break;                   // entity event

        case 0x2D: updateEntityPosAngle(); break; // entity position.
        case 0x58: break;                         // entity velocity.
        case 0x46: break;                         // entity angle head. //idk con esto.
        case 0x2E: updateEntityAngle(); break;    // entity rotation.

        case 0x2C: updateEntityPos(); break; // entity teleport less than 8 blocks.
        case 0x6D: break;                    // entity teleport more than 8 blocks.

        case 0x52: break; // center chunk

        case 0x54: break; // spawn position.
        case 0x23: break; // world border.
        case 0x66: break; // play sound.
        case 0x27: break; // particle
        case 0x6E: break; // tick state.
        case 0x6F: break; // step tick.
        case 0x26: break; // sound.

        case 0x20: break; // game event.
        case 0x0B: break; // dificulty
        case 0x3C: break; // player update tab.
        case 0x3B:
            break; // player disconect

        // case 0x00: printf(INFO "packet delimiter" RESET); break; // delimiter of packet.
        case 0x00: break; // delimiter of packet.

        case 0x0D: printf(INFO "Batch chunks start" RESET); break;
        case 0x0C: printf(INFO "Batch chunks end" RESET); break;

        default: printf(DEBUG "id = 0x%02X, packetLen = 0x%02X -> %d" RESET, id, packetLen, packetLen); break;
        }
    } else {
        printf(DEBUG "status = %u, PacketID = 0x%02X, packetLen = 0x%02X -> %d, readBuff = %zu" RESET, status, packetID, packetLen, packetLen, readBuff.size());
        // if(packetID == 0x73) debugBuff = readBuff; //data ascii no se que es.
        // if(packetID == 0x100) debugBuff = readBuff; //menos idea que es esto.
        // if(packetID == 0x49) debugBuff = readBuff; //oh is a PNG aAAAAaa.
        // if(packetID == 0x70) debugBuff = readBuff; // nose que es mucho text nbt
        if(packetID == 0x2B77) debugBuff = readBuff; // chunck hardcoded
    }
}
// auxiliar para usar en lower_bound
bool compareByID(const types::entity_t& a, const types::entity_t& b) {
    return a.ID < b.ID;
}


void Bot::spawnEntity() {
    using namespace packet;

    types::entity_t entity = {
        .ID = decodeVarInt(readBuff),
        .UUID = decodeUUID(readBuff),
        .typeID = decodeVarInt(readBuff),
        .x = decodeDouble(readBuff),
        .y = decodeDouble(readBuff),
        .z = decodeDouble(readBuff),
        .pitch = decodeByte(readBuff),
        .yaw = decodeByte(readBuff),
        .headYaw = decodeByte(readBuff),
        .data = decodeVarInt(readBuff),
        .xVel = decodeShort(readBuff),
        .yVel = decodeShort(readBuff),
        .zVel = decodeShort(readBuff),
    };

    auto ite = std::lower_bound(entityList.begin(), entityList.end(), entity, compareByID);
    entityList.insert(ite, entity);

    printf(INFO "Entity Spawn ID-> 0x%02X , typeID -> 0x%02X , entityCount -> %zu" RESET, entity.ID, entity.typeID, entityList.size());
}

void Bot::removeEntities() {
    uint16_t count = packet::decodeVarInt(readBuff);
    while(count--) {
        types::entity_t entity = {
            .ID = packet::decodeVarInt(readBuff),
        };
        auto ite = std::lower_bound(entityList.begin(), entityList.end(), entity, compareByID);

        if(ite->ID == entity.ID) {
            entityList.erase(ite);
            printf(INFO "Entity remove ID-> 0x%02X , entityCount -> %zu" RESET, entity.ID, entityList.size());
        }
    }
}

void Bot::updateEntityPos() {
    using namespace packet;

    types::entity_t entity = {
        .ID = decodeVarInt(readBuff),
    };

    auto entiPointer = std::lower_bound(entityList.begin(), entityList.end(), entity, compareByID);

    if(entiPointer->ID == entity.ID) {
        entiPointer->x += (double)decodeShort(readBuff) / (128 * 32);
        entiPointer->y += (double)decodeShort(readBuff) / (128 * 32);
        entiPointer->z += (double)decodeShort(readBuff) / (128 * 32);

        entiPointer->onGround = (bool)readBuff[0];
    }
}


void Bot::updateEntityPosAngle() {
    using namespace packet;

    types::entity_t entity = {
        .ID = decodeVarInt(readBuff),
    };

    auto entiPointer = std::lower_bound(entityList.begin(), entityList.end(), entity, compareByID);

    if(entiPointer->ID == entity.ID) {
        entiPointer->x += (double)decodeShort(readBuff) / (128 * 32);
        entiPointer->y += (double)decodeShort(readBuff) / (128 * 32);
        entiPointer->z += (double)decodeShort(readBuff) / (128 * 32);

        entiPointer->yaw = decodeByte(readBuff);
        entiPointer->pitch = decodeByte(readBuff);
        entiPointer->yaw = decodeByte(readBuff);

        entiPointer->onGround = (bool)readBuff[0];
    }
}
void Bot::updateEntityAngle() {
    using namespace packet;

    types::entity_t entity = {
        .ID = decodeVarInt(readBuff),
    };

    auto entiPointer = std::lower_bound(entityList.begin(), entityList.end(), entity, compareByID);

    if(entiPointer->ID == entity.ID) {

        entiPointer->yaw = decodeByte(readBuff);
        entiPointer->pitch = decodeByte(readBuff);
        entiPointer->yaw = decodeByte(readBuff);

        entiPointer->onGround = (bool)readBuff[0];
    }
}

void Bot::syncPlayerPos(void) {
    using namespace packet;
    struct {
        double x;
        double y;
        double z;
        float yaw;
        float pitch;
    } position = {
        .x = decodeDouble(readBuff),
        .y = decodeDouble(readBuff),
        .z = decodeDouble(readBuff),
        .yaw = decodeFloat(readBuff),
        .pitch = decodeFloat(readBuff),
    };

    uint8_t flags = decodeByte(readBuff);
    uint16_t tpid = decodeVarInt(readBuff);

    if(flags & 0x01) position.x += player.position.x;
    if(flags & 0x02) position.y += player.position.y;
    if(flags & 0x04) position.z += player.position.z;
    if(flags & 0x08) position.yaw += player.position.yaw;
    if(flags & 0x10) position.pitch += player.position.pitch;

    memcpy(&player.position, &position, sizeof(position));

    pushVarInt(sendBuff, 0x00);
    pushVarInt(sendBuff, tpid);
    send();
}
