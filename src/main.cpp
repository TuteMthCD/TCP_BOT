#include <boost/asio/io_context.hpp>
#include "bot.h"

int main() {
    using namespace std;

    boost::asio::io_context io;
    Bot bot(io);
    bot.init("127.0.0.1",25565,"tute","d42755f0f7be48ec9abc21fc1b1f567d",765);
    // bot.init("127.0.0.1",39311,"tute","d42755f0f7be48ec9abc21fc1b1f567d",765);
    
    while (true) {
        sleep(5);
    }
    // getchar();
    return 0;
}
