#include <iostream>
#include <thread>

#include <hydrogen/nio/nio.h>

using namespace nio;

#pragma comment(lib, "nio")

int main(){
    auto ep = endpoint::localhost(7070);

    WSADATA d = { 0 };
    ::WSAStartup(MAKEWORD(2, 2), &d);

    auto f1 = [&ep]()->void {
        try {
            socket_stream stream;
            stream.open(ep);

            char buf[1024] = { 0 };
            buf[0] = 'A';
            buf[1022] = 'Z';
            for (int i = 0; i < 10000; ++i) {
                stream.write(buf, 1024);
                stream.read(buf, 1024);
            }
        }
        catch (nio::exception e) {
            std::cerr << "f1 exception: " << e.what() << '\n';
        }
    };

    auto f2 = [&ep]()->void {
        try {
            socket_acceptor acceptor;
            acceptor.listen(ep);

            socket_stream stream(acceptor.accept());

            char buf[1024];
            for (int i = 0; i < 10000; ++i) {
                stream.read(buf, 1024);
                stream.write(buf, 1024);

                if (buf[0] != 'A' || buf[1022] != 'Z') {
                    std::cout << "pingpong test failed at the " << i + 1 << "th time.\n";
                    break;
                }
            }

        } 
        catch (nio::exception e) {
            std::cerr << "f2 exception: " << e.what() << '\n';
        }
    };

    std::thread th2(f2);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::thread th1(f1);
    
    th1.join();
    th2.join();

    ::WSACleanup();
    return 0;
}
