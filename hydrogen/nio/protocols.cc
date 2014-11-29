#include <hydrogen/nio/protocols.h>

using namespace nio;

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
        throw socket_exception(hy::strcat("Host resolve failed: ", host));
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
    /* bad uniform address string */
    return endpoint();
}

std::string endpoint::to_uname() const {
    char buf[128];
    ::sprintf(buf, "tcp://%d.%d.%d.%d:%d",
            _addr.sin_addr.S_un.S_un_b.s_b1, _addr.sin_addr.S_un.S_un_b.s_b2,
            _addr.sin_addr.S_un.S_un_b.s_b3, _addr.sin_addr.S_un.S_un_b.s_b4,
            (int)::ntohs(_addr.sin_port));
    return std::string(buf);
}
