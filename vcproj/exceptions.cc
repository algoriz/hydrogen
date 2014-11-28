#include <hydrogen/nio/inet_proto.h>
#include <hydrogen/nio/exceptions.h>

using namespace nio;

int nio::error_code::last_error(){
#ifdef _WIN32
    return ::WSAGetLastError() - WSABASEERR;
#else
    return errno;
#endif // _WIN32
}
