#ifndef _DAYLITE_SUBSCRIBER_IMPL_HPP_
#define _DAYLITE_SUBSCRIBER_IMPL_HPP_

#include <functional>
#include <memory>

#include <bson.h>

#include "topic.hpp"
#include "daylite/subscriber.hpp"

#include "packet.hpp"
#include "daylite/result.hpp"
#include "mailbox.hpp"
#include "mailman.hpp"

namespace daylite
{
  class node_impl;

  class subscriber_impl : public subscriber
  {
  public:
    subscriber_impl(node_impl *const parent, const daylite::topic &t, const std::shared_ptr<mailman> &dave,
      subscriber::subscriber_callback_t callback, void* usr_arg);
    ~subscriber_impl();

    inline class daylite::topic topic() const { return _t; }

    // See same method in publisher_impl for more info
    void set_husk();
    
    void set_send_packed(const bool packed) { _send_packed = packed; }
    bool send_packed() const { return _send_packed; }

  private:
    node_impl *_parent;
    daylite::topic _t;
    std::shared_ptr<mailman> _dave;
    std::shared_ptr<mailbox> _mailbox;
    
    bool _husk;
    bool _send_packed;
  };
}

#endif
