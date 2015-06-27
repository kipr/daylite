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
    typedef std::function<void_result(std::shared_ptr<packet>)> incoming_mail_callback_t;
    typedef std::function<void_result(std::unique_ptr<packet>)> outgoing_mail_callback_t;

    mailbox(topic topic
      , incoming_mail_callback_t incoming_mail_callback = incoming_mail_callback_t()
      , outgoing_mail_callback_t outgoing_mail_callback = outgoing_mail_callback_t());

    void_result set_incoming_mail_callback(incoming_mail_callback_t incoming_mail_callback);
    void_result set_outgoing_mail_callback(outgoing_mail_callback_t outgoing_mail_callback);

    topic get_topic() const { return _topic; }
    size_t get_id() const;

    void_result place_incoming_mail(std::shared_ptr<packet> packet);
    void_result place_outgoing_mail(std::unique_ptr<packet> packet);

  private:
    topic _topic;
    incoming_mail_callback_t _incoming_mail_callback;
    outgoing_mail_callback_t _outgoing_mail_callback;
  };
}

#endif
