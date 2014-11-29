#include <hydrogen/nio/protocols.h>
#include <hydrogen/nio/exceptions.h>

using namespace nio;

int nio::last_error(){
#ifdef _WIN32
    return ::WSAGetLastError() - WSABASEERR;
#else
    return errno;
#endif // _WIN32
}
