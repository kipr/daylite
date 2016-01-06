#include "subscriber_impl.hpp"
#include "node_impl.hpp"

using namespace daylite;
using namespace std;

subscriber_impl::subscriber_impl(node_impl *const parent, const daylite::topic &t, const shared_ptr<mailman> &dave,
  subscriber::subscriber_callback_t callback, void *usr_arg)
  : _parent(parent)
  , _t(t)
  , _dave(dave)
  , _mailbox(make_shared<mailbox>(t))
  , _husk(false)
  , _send_packed(false)
{
  _parent->register_subscriber(this);
  _dave->register_mailbox(_mailbox);
  _mailbox->set_incoming_mail_callback([callback, usr_arg, this](const packet &p)
  {
    if(callback) callback(_send_packed ? p.packed() : p.msg(), usr_arg);
    return success();
  });
}

subscriber_impl::~subscriber_impl()
{
  if(_husk) return;
  _parent->unregister_subscriber(this);
  _dave->unregister_mailbox(_mailbox);
}


void subscriber_impl::set_husk()
{
  _dave->unregister_mailbox(_mailbox);
  _dave.reset();
  _mailbox.reset();
  _parent = 0;
  _husk = true;
}