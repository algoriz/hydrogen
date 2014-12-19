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

    hostent* ent = gethostbyname(host);
    if (ent && ent->h_addr_list[0]){
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = *reinterpret_cast<u_long*>(ent->h_addr_list[0]);
        _addr.sin_port = htons(port);
    }
    else {
        throw host_not_found(host);
    }
}

endpoint endpoint::from_uname(const char* uname){
    char schema[8] = "tcp://";
    const size_t schema_len = 6;

    bool bad_schema = false;
    for (size_t i = 0; i < schema_len; ++i){
        if (tolower(uname[i]) != schema[i]){
            bad_schema = true;
            break;
        }
    }

    if (!bad_schema){
        uname += 6;
        if (const char* p = strchr(uname, ':')){
            std::string host(uname, p - uname);
            ++p;
            return endpoint(host.c_str(), atoi(p));
        }
    }

    throw exception(hy::strcat("Invalid uniform address string: ", uname).c_str());
}

std::string endpoint::to_uname() const {
    char buf[128];
    struct {
        unsigned char b1, b2, b3, b4;
    } addr;
    memcpy(&addr, &(_addr.sin_addr), sizeof(addr));
    sprintf(buf, "tcp://%d.%d.%d.%d:%d", addr.b1, addr.b2, addr.b3, addr.b4,
            (int)ntohs(_addr.sin_port));
    return buf;
}
