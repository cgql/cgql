#ifndef CGQL_SOCKET_H
#define CGQL_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace cgql {
namespace internal {
class Socket {
public:
  Socket();
  ~Socket();
private:
  int status;
  struct addrinfo hints;
  struct addrinfo* servInfo;
};
} // internal
} // cgql

#endif /* ifndef CGQL_SOCKET_H */
