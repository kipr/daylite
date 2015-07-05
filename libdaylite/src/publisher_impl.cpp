#include "publisher_impl.hpp"

using namespace daylite;
using namespace std;

publisher_impl::publisher_impl(const topic &t, const std::shared_ptr<mailman> &dave)
  : _t(t)
  , _dave(dave)
  , _mailbox(make_shared<mailbox>(t))
{
  _dave->register_mailbox(_mailbox);
}

publisher_impl::~publisher_impl()
{
  _dave->unregister_mailbox(_mailbox);
}

void_result publisher_impl::publish(const bson_t *msg)
{
  return publish(make_unique<packet>(_mailbox->topic(), msg));
}

void_result publisher_impl::publish(unique_ptr<packet> packet)
{
  return _mailbox->place_outgoing_mail(move(packet));
}
