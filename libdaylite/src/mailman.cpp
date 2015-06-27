#include "console.hpp"
#include "mailman.hpp"
#include "mailbox.hpp"
#include "assert.hpp"

using namespace daylite;
using namespace std;

void_result mailman::register_mailbox(const shared_ptr<mailbox> &mailbox)
{
  mailbox->set_outgoing_mail_callback([this, mailbox](unique_ptr<packet> p)
  {
    return send(mailbox->get_id(), move(p));
  });
  
  _mailboxes[mailbox->get_topic()][mailbox->get_id()] = mailbox;
  return success();
}

void_result mailman::unregister_mailbox(const shared_ptr<mailbox> &mailbox)
{
  _mailboxes[mailbox->get_topic()][mailbox->get_id()]->set_outgoing_mail_callback(mailbox::outgoing_mail_callback_t());
  _mailboxes[mailbox->get_topic()].erase(mailbox->get_id());

  // remove the topic if id was the last mailbox
  if(!_mailboxes[mailbox->get_topic()].size()) _mailboxes.erase(mailbox->get_topic());
  
  return success();
}

void_result mailman::send(const uint32_t sender_id, unique_ptr<packet> p)
{
  shared_ptr<packet> pack = make_shared<packet>(*p.release());

  // send packet to all registered mailboxes for this topic besides the sender one
  auto it = _mailboxes.find(pack->get_topic());
  if(it != _mailboxes.end())
  {
    for(const auto &mailbox : it->second)
    {
      if(mailbox.first == sender_id) continue;
      assert_result(mailbox.second->place_incoming_mail(pack));
    }
  }

  // send packet to all registered mailboxes for the '*' topic besides the sender one
  it = _mailboxes.find(topic::any);
  if(it != _mailboxes.end())
  {
    for(const auto &mailbox : it->second)
    {
      if(mailbox.first == sender_id) continue;
      assert_result(mailbox.second->place_incoming_mail(pack));
    }
  }

  return success();
}
