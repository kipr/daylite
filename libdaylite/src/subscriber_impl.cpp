#include "subscriber_impl.hpp"

using namespace daylite;
using namespace std;

subscriber_impl::subscriber_impl(const topic &t, const shared_ptr<mailman> &dave,
  subscriber::subscriber_callback_t callback, void *usr_arg)
  : _t(t)
  , _dave(dave)
  , _mailbox(make_shared<mailbox>(t))
{
  _dave->register_mailbox(_mailbox);
  _mailbox->set_incoming_mail_callback([callback, usr_arg](const shared_ptr<packet> &packet)
  {
    if(callback) callback(packet->msg(), usr_arg);
    return success();
  });
}

subscriber_impl::~subscriber_impl()
{
  _dave->unregister_mailbox(_mailbox);
}