#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>

// using namespace boost::asio;

class Bot {
    public:
    Bot(std::string addr, unsigned short port, std::string name, std::string uuid, int protocol);
        
    private:
    void pushVarInt(short);
    void read(void);
    void send(void);

    void loginPacket();
    //basicas
    std::string addr;
    unsigned int port;
    std::string name;
    std::string uuid;
    int protocol;

    //buffer necesesario para recibir
    std::vector<uint8_t>buff;

    //socket tcp
    boost::asio::ip::tcp::socket socket;
    boost::asio::io_context io;
};
