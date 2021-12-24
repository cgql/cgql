#include "cgql/net/socket.h"

#include "cgql/utilities/assert.h"

#include <cstring>

namespace cgql {
namespace internal {

Socket::Socket() {
  memset(&this->hints, 0, sizeof(this->hints));
  this->hints.ai_family = AF_UNSPEC;
  this->hints.ai_socktype = SOCK_STREAM;
  this->hints.ai_flags = AI_PASSIVE;

  if((this->status =
    getaddrinfo(
      NULL,
      "6556",
      &this->hints,
      &this->servInfo)
  ) != 0) {
    cgqlAssert(false, "Setting up socket failed");
  }
}
Socket::~Socket() {
  freeaddrinfo(this->servInfo);
}

} // internal
} // cgql
