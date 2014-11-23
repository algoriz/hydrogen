#pragma once
#include <string>

#ifdef WIN32
#include <WinSock2.h>
#else
#include <unix/socket.h>
#endif

class socket_exception : public std::exception {
public:
    explicit socket_exception(int error)
        : _error(error){}

    socket_exception(const char* msg, int error)
        : _error(error), std::exception(msg) {}

    int error_code() const { return _error; }
private:
    int _error;
};

class iosocket {
public:
    explicit iosocket(int fd = -1) : _fd(fd) {}

    ~iosocket() { close(); }

    /* Enable move */
    iosocket(iosocket&& sock) : _fd(sock._fd){ sock._fd = -1; }

    /* Disable copy and assignment */
    iosocket(const iosocket& sock) = delete;
    iosocket& operator= (const iosocket& sock) = delete;

    int read_some(char* buf, size_t len, int flag = 0) {
        return ::recv(_fd, buf, len, flag);
    }

    int write_some(const char* buf, size_t len, int flag = 0) {
        return ::send(_fd, buf, len, flag);
    }

    void read(char* buf, size_t len) throw (socket_exception) {
        while (len) {
            int r = read_some(buf, len);
            if (r > 0) {
                buf += r;
                len -= r;
            }
            else {
                throw socket_exception("network error.", r);
            }
        }
    }

    void write(const char* buf, size_t len) throw (socket_exception) {
        while (len) {
            int w = write_some(buf, len);
            if (w > 0) {
                buf += w;
                len -= w;
            } else {
                throw socket_exception("network error.", w);
            }
        }
    }

    bool is_bad() const { return _fd == -1; }

    void move_to(iosocket& sock) {
        sock._fd = _fd;
        _fd = -1;
    }

    void close() {
        if (!is_bad()) {
            ::shutdown(_fd, SD_BOTH);
            ::closesocket(_fd);
        }
    }

private:
    int _fd;
};
