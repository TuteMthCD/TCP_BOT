#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <mutex>
#include <sys/socket.h>
#include <thread>

// using namespace boost::asio;

class Bot {
    public:
    Bot(boost::asio::io_context& io) : socket(io){};
    void init(std::string addr, unsigned short port, std::string name, std::string uuid, int protocol);

    private:
    //create buff;
    void pushVarInt(short);
    void pushString(std::string);
    void pushUShort(unsigned short);
    void pushUUID(void);

    //decode buff; 
    uint16_t decodeVarInt(void);
    
    //handlers
    bool read(void);
    void send(void);

    void handler(void);
    
    void loginHandler(void);
    void configHandler(void);
    void playHandler(void);
    
    void loginPacket(void);
    // basicas
    std::string addr;
    unsigned int port;
    std::string name;
    std::string uuid;
    int protocol;
    // buffer necesesario para recibir
    std::vector<uint8_t> sendBuff;
    std::vector<uint8_t> readBuff;

    // socket tcp
    boost::asio::ip::tcp::socket socket;
    std::thread thread;
    bool th_stop = false;
    std::mutex mtxSocket;

    // otras
    enum{
        login,
        config,
        play,
    }status = login;

    uint16_t compression_threshold = 256; // 256 is default.
};
