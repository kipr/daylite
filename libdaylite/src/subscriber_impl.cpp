#include "subscriber_impl.hpp"

using namespace daylite;
using namespace std;

subscriber_impl::subscriber_impl(const shared_ptr<mailbox> &mailbox, subscriber::subscriber_callback_t callback, void* usr_arg)
  : _mailbox(mailbox)
{
  _mailbox->set_incoming_mail_callback([callback,usr_arg](const shared_ptr<packet> &packet)
  {
    if(callback) callback(packet->get_msg(), usr_arg);
    return success();
  });
}
