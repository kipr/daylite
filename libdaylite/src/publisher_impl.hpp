#ifndef _DAYLIGHT_PUBLISHER_IMPL_HPP_
#define _DAYLIGHT_PUBLISHER_IMPL_HPP_

#include <memory>

#include "daylite/bson.hpp"

#include "daylite/publisher.hpp"

#include "topic.hpp"
#include "packet.hpp"
#include "daylite/result.hpp"
#include "mailbox.hpp"
#include "mailman.hpp"

#include "splat.hpp"

namespace daylite
{
  class node_impl;

  class publisher_impl : public publisher
  {
  public:
    publisher_impl(node_impl *const parent, const daylite::topic &t, const std::shared_ptr<mailman> &dave);
    ~publisher_impl();

    unsigned subscriber_count();
    void_result publish(const bson &msg);
    void_result publish(packet p);
    
    inline bool firehose() const { return _firehose; }
    inline void set_firehose(const bool firehose) { _firehose = firehose; }
    
    inline class daylite::topic topic() const { return _t; }
    
    // This is called by the parent node_impl
    // if it goes out of scope before its
    // child classes
    void set_husk();

  private:
    node_impl *_parent;
    daylite::topic _t;
    std::shared_ptr<mailman> _dave;
    std::shared_ptr<mailbox> _mailbox;
    bool _firehose;
    
    bool _husk;
  };
}

#endif
