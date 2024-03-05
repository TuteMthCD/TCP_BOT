#include "bot.h"
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>


Bot::Bot(std::string _addr, unsigned short _port, std::string _name, std::string _uuid, int _protocol)
: io(), socket(io) {
    addr = _addr;
    port = _port;
    name = _name;
    uuid = _uuid;
    protocol = _protocol;

    boost::asio::ip::address addr = boost::asio::ip::address::from_string(_addr);
    socket.connect(boost::asio::ip::tcp::endpoint(addr,port));
    // loginPacket();
}
void Bot::pushVarInt(short shortValue) {
    do {
        char byte = shortValue & 0x7F; // Obtener los 7 bits menos significativos
        shortValue >>= 7;              // Desplazar los bits restantes
        if(shortValue != 0) {          // Si quedan más bytes por codificar
            byte |= 0x80;              // Establecer el bit más significativo a 1 para indicar más bytes
        }
        buff.push_back(byte); // Agregar el byte al vector
    } while(shortValue != 0);
}

void Bot::read(void) {
    uint8_t length_buffer;
    boost::asio::read(socket, boost::asio::buffer(&length_buffer, 1));
    // Read the rest of the string based on the determined length
    buff.clear();
    buff.resize(length_buffer);
    boost::asio::read(socket, boost::asio::buffer(buff));
}

void Bot::send() {
    buff.insert(buff.begin(), buff.size());
    write(socket, boost::asio::buffer(buff));
    buff.clear();
}
