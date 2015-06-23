#include "subscriber_impl.hpp"

using namespace daylite;

subscriber_impl::subscriber_impl(std::shared_ptr<mailbox> mailbox, subscriber::subscriber_callback_t callback)
  : _mailbox(mailbox)
{
  _mailbox->set_incoming_mail_callback([callback](std::shared_ptr<packet> packet)
  {
    if(callback) callback(packet->get_msg());
    return success();
  });
}
