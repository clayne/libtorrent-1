#include "config.h"

#include "torrent/net/http_stack.h"

#include <cassert>

#include "net/curl_get.h"
#include "net/curl_stack.h"
#include "net/thread_net.h"
#include "torrent/exceptions.h"
#include "torrent/net/http_get.h"
#include "torrent/utils/thread.h"

namespace torrent::net {

HttpStack::HttpStack(utils::Thread* thread) :
    m_stack(new CurlStack(thread)) {
}

HttpStack::~HttpStack() = default;

void
HttpStack::shutdown() {
  m_stack->shutdown();
}

void
HttpStack::start_get(HttpGet& http_get) {
  if (!http_get.is_valid())
    throw torrent::internal_error("HttpStack::start_get() called with an invalid HttpGet object.");

  http_get.curl_get()->set_was_started();

  auto curl_get_weak = std::weak_ptr<CurlGet>(http_get.curl_get());

  m_stack->thread()->callback(nullptr, [stack = m_stack.get(), curl_get_weak]() {
      auto curl_get = curl_get_weak.lock();

      if (curl_get)
        stack->start_get(curl_get);
    });
}

unsigned int
HttpStack::active() const {
  return m_stack->active();
}

unsigned int
HttpStack::max_active() const {
  return m_stack->max_active();
}

void
HttpStack::set_max_active(unsigned int a) {
  m_stack->set_max_active(a);
}

std::string
HttpStack::user_agent() const {
  return m_stack->user_agent();
}

std::string
HttpStack::http_proxy() const {
  return m_stack->http_proxy();
}

std::string
HttpStack::bind_address() const {
  return m_stack->bind_address();
}

std::string
HttpStack::http_capath() const {
  return m_stack->http_capath();
}

std::string
HttpStack::http_cacert() const {
  return m_stack->http_cacert();
}

void
HttpStack::set_user_agent(const std::string& s) {
  m_stack->set_user_agent(s);
}

void
HttpStack::set_http_proxy(const std::string& s) {
  m_stack->set_http_proxy(s);
}

void
HttpStack::set_bind_address(const std::string& s) {
  m_stack->set_bind_address(s);
}

void
HttpStack::set_http_capath(const std::string& s) {
  m_stack->set_http_capath(s);
}

void
HttpStack::set_http_cacert(const std::string& s) {
  m_stack->set_http_cacert(s);
}

bool
HttpStack::ssl_verify_host() const {
  return m_stack->ssl_verify_host();
}

bool
HttpStack::ssl_verify_peer() const {
  return m_stack->ssl_verify_peer();
}

void
HttpStack::set_ssl_verify_host(bool s) {
  m_stack->set_ssl_verify_host(s);
}

void
HttpStack::set_ssl_verify_peer(bool s) {
  m_stack->set_ssl_verify_peer(s);
}

long
HttpStack::dns_timeout() const {
  return m_stack->dns_timeout();
}

void
HttpStack::set_dns_timeout(long timeout) {
  m_stack->set_dns_timeout(timeout);
}

} // namespace torrent::net
