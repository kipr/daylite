#include "publisher_impl.hpp"
#include "node_impl.hpp"

using namespace daylite;
using namespace std;

publisher_impl::publisher_impl(node_impl *const parent, const daylite::topic &t, const std::shared_ptr<mailman> &dave)
  : _parent(parent)
  , _t(t)
  , _dave(dave)
  , _mailbox(make_shared<mailbox>(t))
  , _firehose(false)
  , _husk(false)
{
  _parent->register_publisher(this);
  _dave->register_mailbox(_mailbox);
}

publisher_impl::~publisher_impl()
{
  if(_husk) return;
  _parent->unregister_publisher(this);
  _dave->unregister_mailbox(_mailbox);
}

unsigned publisher_impl::subscriber_count()
{
  if(_husk) return 0;
  auto it = _parent->_subscription_count.find(_t.name());
  if(it == _parent->_subscription_count.end()) return 0U;
  return it->second;
}

void_result publisher_impl::publish(const bson &msg)
{
  if(_husk) return failure("Husk");
  _parent->update_time();
  return publish(packet(_mailbox->topic(), _parent->time(), msg, false));
}

void_result publisher_impl::publish(packet p)
{
  p.meta().droppable = _firehose;
  p.meta().origin_id = _parent->_id;
  p.build();
  return _mailbox->place_outgoing_mail(p);
}

void publisher_impl::set_husk()
{
  _dave->unregister_mailbox(_mailbox);
  _dave.reset();
  _mailbox.reset();
  _parent = 0;
  _husk = true;
}