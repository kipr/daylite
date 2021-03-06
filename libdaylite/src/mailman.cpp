#include "console.hpp"
#include "mailman.hpp"
#include "mailbox.hpp"
#include "assert.hpp"

using namespace daylite;
using namespace std;

void_result mailman::register_mailbox(const shared_ptr<mailbox> &mailbox)
{
  mailbox->set_outgoing_mail_callback([this, mailbox](const packet &p)
  {
    return send(mailbox->id(), p);
  });
  
  _mailboxes[mailbox->topic()][mailbox->id()] = mailbox;
  return success();
}

void_result mailman::unregister_mailbox(const shared_ptr<mailbox> &mailbox)
{
  _mailboxes[mailbox->topic()][mailbox->id()]->set_outgoing_mail_callback(mailbox::outgoing_mail_callback_t());
  _mailboxes[mailbox->topic()].erase(mailbox->id());

  // remove the topic if id was the last mailbox
  if(!_mailboxes[mailbox->topic()].size()) _mailboxes.erase(mailbox->topic());
  
  return success();
}

void_result mailman::send(const uint32_t sender_id, const packet &p)
{
  // send packet to all registered mailboxes for this topic besides the sender one
  auto it = _mailboxes.find(p.topic());
  if(it != _mailboxes.end())
  {
    for(const auto &mailbox : it->second)
    {
      if(mailbox.first == sender_id) continue;
      assert_result(mailbox.second->place_incoming_mail(p));
    }
  }

  // send packet to all registered mailboxes for the '*' topic besides the sender one
  it = _mailboxes.find(topic::any);
  if(it != _mailboxes.end())
  {
    for(const auto &mailbox : it->second)
    {
      if(mailbox.first == sender_id) continue;
      assert_result(mailbox.second->place_incoming_mail(p));
    }
  }

  return success();
}

void mailman::clear()
{
  _mailboxes.clear();
}
