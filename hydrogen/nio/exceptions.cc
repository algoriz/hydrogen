#include <hydrogen/nio/protocols.h>
#include <hydrogen/nio/exceptions.h>

int hy::socket_error::last(){
#ifdef _WIN32
    return ::WSAGetLastError() - WSABASEERR;
#else
    return errno;
#endif // _WIN32
}
