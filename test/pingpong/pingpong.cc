#include <iostream>
#include <thread>

#include <hydrogen/nio/nio.h>

using namespace nio;

#pragma comment(lib, "nio")

class EchoServer {
public:
    class EchoServerClient {
    public:
        EchoServerClient(stream_socket&& s)
            : _client(std::move(s)){
        }

        void operator()(){ run(); }

        void run(){
            char buf[4096];
            try {
                for (int i = 0; i < 10000; ++i) {
                    auto size = _client.read_some(buf, sizeof(buf));
                    _client.write(buf, size);
                }
            }
            catch (nio::socket_exception e){
                std::cerr << "EchoServerClient exception out: " << e.what() << '\n';
            }
        }

    private:
        socket_stream _client;
    };

    EchoServer(endpoint ep): _name(ep){}

    void operator()(){ run(); }

    void run(){
        try {
            socket_acceptor acceptor;
            acceptor.listen(_name);

            while (true){
                EchoServerClient c(acceptor.accept());
                c.run();
            }
        }
        catch (nio::socket_exception e) {
            std::cerr << "EchoServer exception: " << e.what() << '\n';
        }
        catch (...){
            std::cerr << "unknown exception captured.\n";
        }
    }

private:
    endpoint _name;
};

class PingpongTest {
public:
    PingpongTest(endpoint ep) : _name(ep){}
    void operator()() {
        try {
            run();
        }
        catch (nio::socket_exception e){
            std::cerr << "PingpongTest exception out, " << e.what() << '\n';
        }
    }

    void run() {
        socket_stream stream(_name);
        char wbuf[4096];
        char rbuf[sizeof(wbuf)];

        int size = 16;
        std::chrono::high_resolution_clock clk;
        auto t1 = clk.now();
        for (int i = 0; i < 10000; ++i) {
            memset(wbuf, 0x61616161, size);
            wbuf[0] = '(';
            wbuf[size - 1] = ')';
            stream.write(wbuf, size);
            stream.read(rbuf, size);

            if (memcmp(wbuf, rbuf, size)){
                std::cout << "PingpongTest failed at the " << i + 1 << "th loop.\n";
                break;
            }

            size *= 2;
            if (size > sizeof(wbuf)){
                size = sizeof(wbuf);
            }
        }
        auto tm = clk.now() - t1;
        std::cout << "IO time: " << tm.count() << '\n';
        std::cout << "PingpongTest send: " << stream.tellg() << ", recv: " << stream.tellp() << '\n';
    }

private:
    endpoint _name;
};

class LineByLineTest {
public:
    LineByLineTest(endpoint ep) : _name(ep){}
    void operator()() {
        try {
            run();
        }
        catch (nio::socket_exception e){
            std::cerr << "LineByLineTest exception out, " << e.what() << '\n';
        }
    }

    void run(){
        socket_stream s;
        s.open(_name);

        char l0[] = "";
        char l1[] = "1";
        char l2[] = "12";
        char l3[] = "123";
        char l4[] = "1234";
        std::string l5000;
        l5000.assign(5000, '*');

        s.write("\n", 1);
        s.write(l1, 1);
        s.write("\n", 1);
        s.write(l2, 2);
        s.write("\n", 1);
        s.write(l3, 3);
        s.write("\n", 1);
        s.write(l4, 4);
        s.write("\n", 1);

        char buf[5004];
        s.getline(buf, 100);
        validate(!strcmp(buf, l0), "LineByLineTest #1");
        s.getline(buf, 100);
        validate(!strcmp(buf, l1), "LineByLineTest #2");
        s.getline(buf, 100);
        validate(!strcmp(buf, l2), "LineByLineTest #3");
        s.getline(buf, 100);
        validate(!strcmp(buf, l3), "LineByLineTest #4");
        s.getline(buf, 100);
        validate(!strcmp(buf, l4), "LineByLineTest #5");

        s.write(l5000.c_str(), l5000.length());
        s.write("\n", 1);
        s.getline(buf, 5004);
        validate(l5000 == buf, "LineByLineTest #5");
    }

    void validate(bool r, const char* test){
        std::cout << test << ": " << (r ? "PASSED\n" : "FAILED\n");
    }
private:
    endpoint _name;
};


int main(){
    WSADATA d = { 0 };
    ::WSAStartup(MAKEWORD(2, 2), &d);
    
    endpoint ep = endpoint::localhost(7070);


    std::thread th2([&ep]()->void {
        EchoServer server(ep);
        server.run();
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::thread th1([&ep]()->void {
        PingpongTest pptest(ep);
        pptest.run();

        LineByLineTest lbltest(ep);
        lbltest.run();
    });
    
    th1.join();
    th2.join();

    ::WSACleanup();
    return 0;
}
