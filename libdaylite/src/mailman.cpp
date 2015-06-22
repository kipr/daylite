#include "console.hpp"
#include "mailman.hpp"
#include "mailbox.hpp"

using namespace daylite;

void_result mailman::register_mailbox(std::shared_ptr<mailbox> mailbox)
{
  mailbox->set_outgoing_mail_callback([this](std::unique_ptr<packet> p) { return send(std::move(p)); });
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

void_result mailman::send(std::unique_ptr<packet> p)
{
  if(p->size() < 1) return failure("invalid packet");
 // if(p->data()[0] != 0) return failure("invalid packet");

  std::shared_ptr<packet> pack = std::make_shared<packet>(*p.release());

  // check if mailboxes are registered for this topic
  // workaround: topic '*' are from the network, deliver it just to named topics
  if(pack->get_topic() == topic::any)
  {
    for(auto mb : _mailboxes)
    {
      if(mb.first != topic::any)
      {
        for(auto mailbox : mb.second)
        {
          mailbox.second->place_incoming_mail(pack);
        }
      }
    }
  }
  // workaround: topic '/xxx' are from local, deliver it just to network topics
  else
  {
    for(auto mb : _mailboxes)
    {
      if(mb.first == topic::any)
      {
        for(auto mailbox : mb.second)
        {
          mailbox.second->place_incoming_mail(pack);
        }
      }
    }
  }

  return success();
}
