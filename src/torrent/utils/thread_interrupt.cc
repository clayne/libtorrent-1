#include "config.h"

#include "thread_interrupt.h"

#include <sys/socket.h>

#include "net/socket_fd.h"
#include "rak/error_number.h"
#include "torrent/exceptions.h"
#include "utils/instrumentation.h"

namespace torrent {

thread_interrupt::thread_interrupt(int fd) :
  m_poking(false) {
  m_fileDesc = fd;
  get_fd().set_nonblock();
}

thread_interrupt::~thread_interrupt() {
  if (m_fileDesc == -1)
    return;

  ::close(m_fileDesc);
  m_fileDesc = -1;
}

bool
thread_interrupt::poke() {
  bool expected = false;
  if (!m_other->m_poking.compare_exchange_strong(expected, true))
    return true;

  int result = ::send(m_fileDesc, "a", 1, 0);

  if (result == 0 ||
      (result == -1 && !rak::error_number::current().is_blocked_momentary()))
    throw internal_error("Invalid result writing to thread_interrupt socket.");

  instrumentation_update(INSTRUMENTATION_POLLING_INTERRUPT_POKE, 1);

  return true;
}

thread_interrupt::pair_type
thread_interrupt::create_pair() {
  int fd1, fd2;

  if (!SocketFd::open_socket_pair(fd1, fd2))
    throw internal_error("Could not create socket pair for thread_interrupt: " + std::string(rak::error_number::current().c_str()) + ".");

  pair_type result;

  result.first = std::unique_ptr<thread_interrupt>(new thread_interrupt(fd1));
  result.second = std::unique_ptr<thread_interrupt>(new thread_interrupt(fd2));

  result.first->m_other = result.second.get();
  result.second->m_other = result.first.get();

  return result;
}

void
thread_interrupt::event_read() {
  char buffer[256];
  int result = ::recv(m_fileDesc, buffer, 256, 0);

  if (result == 0 ||
      (result == -1 && !rak::error_number::current().is_blocked_momentary()))
    throw internal_error("Invalid result reading from thread_interrupt socket.");

  instrumentation_update(INSTRUMENTATION_POLLING_INTERRUPT_READ_EVENT, 1);

  m_poking = false;
}

}
