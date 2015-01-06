#include <hydrogen/nio/protocols.h>
#include <hydrogen/common/string.h>

using namespace hy;

#ifdef WIN32
namespace {
    class winsock_init {
    public:
        winsock_init(){
            WSADATA wd = { 0 };
            ::WSAStartup(MAKEWORD(2, 2), &wd);
        }

        ~winsock_init(){
            ::WSACleanup();
        }
    };
    static winsock_init init;
}
#endif // WIN32

endpoint::endpoint(){
    memset(&_addr, 0, sizeof(_addr));
}

endpoint::endpoint(const char* host, int port){
    _setendpoint(host, port);
}

endpoint::endpoint(const char* uname){
    auto part = string(uname).trim().split_n<2>(':');
    if (part[0].empty() || part[0].length() > 127 || part[1].empty()){
        throw io_exception("Bad address string.");
    }

    char host[128];
    _setendpoint(part[0].copy(host), part[1].to_int());
}

std::string endpoint::name() const {
    char buf[128];
    struct {
        unsigned char b1, b2, b3, b4;
    } addr;
    memcpy(&addr, &(_addr.sin_addr), sizeof(addr));
    sprintf(buf, "%d.%d.%d.%d:%d", addr.b1, addr.b2, addr.b3, addr.b4,
            (int)ntohs(_addr.sin_port));
    return buf;
}

void hy::endpoint::_setendpoint(const char* host, int port){
    memset(&_addr, 0, sizeof(_addr));

    hostent* ent = gethostbyname(host);
    if (ent && ent->h_addr_list[0]){
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = *reinterpret_cast<u_long*>(ent->h_addr_list[0]);
        _addr.sin_port = htons(port);
    }
    else {
        throw io_exception("Host not found");
    }
}
