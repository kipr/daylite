#include "mailbox.hpp"

using namespace daylite;
using namespace std;

mailbox::mailbox(const class topic &topic
  , const incoming_mail_callback_t &incoming_mail_callback
  , const outgoing_mail_callback_t &outgoing_mail_callback)
  : _topic(move(topic))
  , _incoming_mail_callback(move(incoming_mail_callback))
  , _outgoing_mail_callback(move(outgoing_mail_callback))
{

}

void_result mailbox::set_incoming_mail_callback(const incoming_mail_callback_t &incoming_mail_callback)
{
  _incoming_mail_callback = move(incoming_mail_callback);
  return success();
}

void_result mailbox::set_outgoing_mail_callback(const outgoing_mail_callback_t &outgoing_mail_callback)
{
  _outgoing_mail_callback = move(outgoing_mail_callback);
  return success();
}

size_t mailbox::id() const
{
  return (size_t) this;
}

void_result mailbox::place_incoming_mail(const packet &p)
{
  if(_incoming_mail_callback) return _incoming_mail_callback(p);
  
  return success();
}

void_result mailbox::place_outgoing_mail(const packet &p)
{
  if(_outgoing_mail_callback) return _outgoing_mail_callback(p);
  return success();
}
