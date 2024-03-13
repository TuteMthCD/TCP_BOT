#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <zlib.h>

#include "varTypes.h"

#define ERROR "\033[1;31m[E]-> " // Red
#define INFO "\033[1;32m[I]-> "
#define DEBUG "\033[1;33m[D]-> "
#define RESET "\033[0m\n"
#define BLUE_COLOR "\033[0;34m"
#define PACKET "\033[0;35m[P]->" // magenta
#define CYAN_COLOR "\033[0;36m"
#define WHITE_COLOR "\033[0;37m" // White

// using namespace boost::asio;

class Bot {
    public:
    Bot(boost::asio::io_context& _io) : socket(_io) {
        io = &_io;
    };
    void init(std::string addr, unsigned short port, std::string name, std::string uuid, int protocol);

    struct {
        long timeDay = 0;
        long worldAge = 0;
    } tickTime;

    struct {
#pragma pack(push, 1) // para crear un struct de 9 bytes
        struct {
            float foodSat;
            uint8_t food;
            float hp;
        } healt;
#pragma pack(pop)
    } player;

    private:
    // handlers
    void send(void);
    void read(void);


    void run(void);
    void handler(const boost::system::error_code& err, std::size_t bytes_transferred);

    void loginHandler(void);
    void configHandler(void);
    void playHandler(void);

    void loginPacket(void);
    void uncompressPacket(void);
    void decodePacketLength(void);

    // basicas
    std::string addr;
    unsigned int port;
    std::string name;
    std::string uuid;
    int protocol;
    // buffer necesesario para recibir
    std::vector<uint8_t> sendBuff;
    std::vector<uint8_t> readBuff;
    uint32_t packetLen;
    // socket tcp
    boost::asio::ip::tcp::socket socket;
    boost::asio::io_context* io;
    std::thread th;
    // otras
    enum {
        login,
        config,
        play,
    } status = login;

    uint32_t packetID;
    uint32_t id;
    uint32_t compression_threshold = 256; // 256 is default.
};