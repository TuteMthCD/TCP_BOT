#include <boost/asio.hpp>
#include <cstdio>

using namespace boost::asio;
using ip::tcp;

void pushBackVarInt(std::vector<uint8_t>& charVector, short shortValue) {
    do {
        char byte = shortValue & 0x7F; // Obtener los 7 bits menos significativos
        shortValue >>= 7;              // Desplazar los bits restantes
        if(shortValue != 0) {          // Si quedan más bytes por codificar
            byte |= 0x80;              // Establecer el bit más significativo a 1 para indicar más bytes
        }
    charVector.push_back(byte); // Agregar el byte al vector
    } while(shortValue != 0);
}

void read_socket(std::vector<uint8_t>& _buffer, tcp::socket& _socket) {
    uint8_t length_buffer;
    boost::asio::read(_socket, boost::asio::buffer(&length_buffer, 1));
    // Read the rest of the string based on the determined length
    _buffer.clear();
    _buffer.resize(length_buffer);
    boost::asio::read(_socket, boost::asio::buffer(_buffer));

    printf("\n\nBuff\n"); 
    for(uint8_t c : _buffer) printf("0x%02X, ", c);
}

void send_socket(std::vector<uint8_t>& _buffer, tcp::socket& _socket) {
    _buffer.insert(_buffer.begin(), _buffer.size());
    write(_socket, boost::asio::buffer(_buffer));
    _buffer.clear();

}

void init_packet(std::string _addr, unsigned short _port, std::string _name, std::string uuid, int _protocol, tcp::socket& _socket) {
    using namespace std;

    ip::address addr = ip::address::from_string(_addr);

    _socket.connect(tcp::endpoint(addr, _port));

    vector<uint8_t> buffer;

    //--primer paquete--
    buffer.push_back(0x00);            // propio del protocolo
    pushBackVarInt(buffer, _protocol); // varint del protocolo version

    // agrego el addr como string, con su tamaño antes
    buffer.push_back(_addr.size());
    for(char c : _addr) buffer.push_back(c);

    // paso el puerto como short
    buffer.push_back(_port >> 8);
    buffer.push_back(_port);

    // buffer.push_back(0x01);

    // propio del protocolo
    buffer.push_back(0x02);

    // agrego al principio el tamaño del mensaje
    send_socket(buffer, _socket);
    // lo mando
    buffer.clear(); // limpio eh vector

    //--segundo paquete--
    buffer.push_back(0x00);

    // mando el nombre del player
    buffer.push_back(_name.size());
    for(char c : _name) buffer.push_back(c);

    // paso el uid de string a bytes
    for(size_t i = 0; i < uuid.size(); i += 2) {
        std::string byteString = uuid.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        buffer.push_back(byte);
    }

    send_socket(buffer, _socket);
}

int main() {
    using namespace std;
    try {
        io_context io_context;
        tcp::socket socket(io_context);

        init_packet("127.0.0.1", 25565, "tute", "d42755f0f7be48ec9abc21fc1b1f567d", 765, socket);

        std::vector<uint8_t> buff;

        read_socket(buff, socket);

        read_socket(buff, socket);

        buff.clear();

        buff.push_back(0x00);
        buff.push_back(0x03);

        send_socket(buff, socket);

        read_socket(buff, socket);
        send_socket(buff, socket);

    } catch(std::exception& e) { printf("%s\n",e.what()); }

    return 0;
}
