#include "console.hpp"
#include "mailman.hpp"
#include "mailbox.hpp"

using namespace daylite;

void_result mailman::register_mailbox(std::shared_ptr<mailbox> mailbox)
{
  mailbox->set_outgoing_mail_callback([this,mailbox](std::unique_ptr<packet> p) { return send(mailbox->get_id(), std::move(p)); });
  _mailboxes[mailbox->get_topic()][mailbox->get_id()] = mailbox;
  return success();
}

void_result mailman::unregister_mailbox(std::shared_ptr<mailbox> mailbox)
{
  _mailboxes[mailbox->get_topic()][mailbox->get_id()]->set_outgoing_mail_callback(mailbox::outgoing_mail_callback_t());
  _mailboxes[mailbox->get_topic()].erase(mailbox->get_id());

  // remove the topic if id was the last mailbox
  if(_mailboxes[mailbox->get_topic()].size() == 0)
  {
    _mailboxes.erase(mailbox->get_topic());
  }

  return success();
}

void_result mailman::send(uint32_t sender_id, std::unique_ptr<packet> p)
{
  std::shared_ptr<packet> pack = std::make_shared<packet>(*p.release());

  // send packet to all registered mailboxes for this topic besides the sender one
  auto it = _mailboxes.find(pack->get_topic());
  if(it != _mailboxes.end())
  {
    for(auto mailbox : it->second)
    {
      if(mailbox.first != sender_id)
      {
        mailbox.second->place_incoming_mail(pack);
      }
    }
  }

  // send packet to all registered mailboxes for the '*' topic besides the sender one
  it = _mailboxes.find(topic::any);
  if(it != _mailboxes.end())
  {
    for(auto mailbox : it->second)
    {
      if(mailbox.first != sender_id)
      {
        mailbox.second->place_incoming_mail(pack);
      }
    }
  }

  return success();
}
