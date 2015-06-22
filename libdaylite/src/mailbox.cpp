#include "mailbox.hpp"

using namespace daylite;

mailbox::mailbox(topic topic
  , incoming_mail_callback_t incoming_mail_callback
  , outgoing_mail_callback_t outgoing_mail_callback)
  : _topic(std::move(topic))
  , _incoming_mail_callback(std::move(incoming_mail_callback))
  , _outgoing_mail_callback(std::move(outgoing_mail_callback))
{

}

void_result mailbox::set_incoming_mail_callback(incoming_mail_callback_t incoming_mail_callback)
{
  _incoming_mail_callback = std::move(incoming_mail_callback);
  return success();
}

void_result mailbox::set_outgoing_mail_callback(outgoing_mail_callback_t outgoing_mail_callback)
{
  _outgoing_mail_callback = std::move(outgoing_mail_callback);
  return success();
}

uint32_t mailbox::get_id() const
{
  return (uint32_t) this;
}

void_result mailbox::place_incoming_mail(std::shared_ptr<packet> packet)
{
  if(_incoming_mail_callback)
  {
    return _incoming_mail_callback(packet);
  }
  else
  {
    return success();
  }
}

void_result mailbox::place_outgoing_mail(std::unique_ptr<packet> packet)
{
  if(_outgoing_mail_callback)
  {
    return _outgoing_mail_callback(std::move(packet));
  }
  else
  {
    return success();
  }
}
