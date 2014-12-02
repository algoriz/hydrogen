#include <hydrogen/nio/protocols.h>

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
    memset(&_addr, 0, sizeof(_addr));

    hostent* ent = ::gethostbyname(host);
    if (ent && ent->h_addr_list[0]){
        _addr.sin_family = AF_INET;
        _addr.sin_addr.S_un.S_addr = *reinterpret_cast<u_long*>(ent->h_addr_list[0]);
        _addr.sin_port = ::htons(port);
    }
    else {
        throw host_not_found(host);
    }
}

endpoint endpoint::from_uname(const char* uname){
    if (!_strnicmp(uname, "tcp://", 6)){
        uname += 6;
        if (const char* p = strchr(uname, ':')){
            std::string host(uname, p - uname);
            ++p;
            return endpoint(host.c_str(), atoi(p));
        }
    }

    throw std::invalid_argument(
        hy::strcat("Invalid uniform address string: ", uname).c_str());
}

std::string endpoint::to_uname() const {
    char buf[128];
    ::sprintf(buf, "tcp://%d.%d.%d.%d:%d",
            _addr.sin_addr.S_un.S_un_b.s_b1, _addr.sin_addr.S_un.S_un_b.s_b2,
            _addr.sin_addr.S_un.S_un_b.s_b3, _addr.sin_addr.S_un.S_un_b.s_b4,
            (int)::ntohs(_addr.sin_port));
    return buf;
}
