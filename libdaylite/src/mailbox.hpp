#ifndef _DAYLITE_MAILBOX_HPP_
#define _DAYLITE_MAILBOX_HPP_

#include "daylite/result.hpp"
#include "packet.hpp"
#include "topic.hpp"

#include <functional>
#include <list>
#include <memory>

namespace daylite
{
  class mailbox
  {
  public:
    typedef std::function<void_result (const packet &)> incoming_mail_callback_t;
    typedef std::function<void_result (const packet &)> outgoing_mail_callback_t;

    mailbox(const topic &topic
      , const incoming_mail_callback_t &incoming_mail_callback = incoming_mail_callback_t()
      , const outgoing_mail_callback_t &outgoing_mail_callback = outgoing_mail_callback_t());

    void_result set_incoming_mail_callback(const incoming_mail_callback_t &incoming_mail_callback);
    void_result set_outgoing_mail_callback(const outgoing_mail_callback_t &outgoing_mail_callback);

    const class topic &topic() const { return _topic; }
    size_t id() const;

    void_result place_incoming_mail(const packet &p);
    void_result place_outgoing_mail(const packet &p);

  private:
    class topic _topic;
    incoming_mail_callback_t _incoming_mail_callback;
    outgoing_mail_callback_t _outgoing_mail_callback;
  };
}

#endif
