#include "publisher_impl.hpp"

using namespace daylite;
using namespace std;

publisher_impl::publisher_impl(const shared_ptr<mailbox> &mailbox)
  : _mailbox(mailbox)
{
}

void_result publisher_impl::publish(const bson_t *msg)
{
  return publish(make_unique<packet>(_mailbox->get_topic(), msg));
}

void_result publisher_impl::publish(unique_ptr<packet> packet)
{
  return _mailbox->place_outgoing_mail(move(packet));
}
