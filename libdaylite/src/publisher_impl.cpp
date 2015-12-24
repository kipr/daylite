#include "publisher_impl.hpp"
#include "node_impl.hpp"

using namespace daylite;
using namespace std;

publisher_impl::publisher_impl(node_impl *const parent, const daylite::topic &t, const std::shared_ptr<mailman> &dave)
  : _parent(parent)
  , _t(t)
  , _dave(dave)
  , _mailbox(make_shared<mailbox>(t))
{
  _parent->register_publisher(this);
  _dave->register_mailbox(_mailbox);
}

publisher_impl::~publisher_impl()
{
  _parent->unregister_publisher(this);
  _dave->unregister_mailbox(_mailbox);
}

void_result publisher_impl::publish(const bson_t *msg)
{
  _parent->update_time();
  return publish(make_unique<packet>(_mailbox->topic(), _parent->time(), msg));
}

void_result publisher_impl::publish(unique_ptr<packet> packet)
{
  return _mailbox->place_outgoing_mail(move(packet));
}
