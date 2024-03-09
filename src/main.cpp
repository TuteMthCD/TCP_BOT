#include <boost/asio/io_context.hpp>
#include <unistd.h>
#include "bot.h"

int main() {
    using namespace std;

    boost::asio::io_context io;
    Bot bot(io);
    bot.init("127.0.0.1",25565,"tute","d42755f0f7be48ec9abc21fc1b1f567d",765);

    // try {
    //     io_context io_context;
    //     tcp::socket socket(io_context);
    //
    //     init_packet("127.0.0.1", 25565, "tute", "d42755f0f7be48ec9abc21fc1b1f567d", 765, socket);
    //
    //     std::vector<uint8_t> buff;
    //
    //     read_socket(buff, socket);
    //
    //     read_socket(buff, socket);
    //
    //     buff.clear();
    //
    //     buff.push_back(0x00);
    //     buff.push_back(0x03);
    //
    //     send_socket(buff, socket);
    //
    //     read_socket(buff, socket);
    //     send_socket(buff, socket);
    //
    // } catch(std::exception& e) { printf("%s\n",e.what()); }
    
    while (true) {
        sleep(5);
    }
    // getchar();
    return 0;
}
