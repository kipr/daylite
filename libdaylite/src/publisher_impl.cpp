#include "publisher_impl.hpp"

using namespace daylite;

publisher_impl::publisher_impl(std::shared_ptr<mailbox> mailbox)
  : _mailbox(mailbox)
{
}

void_result publisher_impl::publish(const bson_t *msg)
{
  return publish(std::make_unique<packet>(_mailbox->get_topic(), msg));
}

void_result publisher_impl::publish(std::unique_ptr<packet> packet)
{
  return _mailbox->place_outgoing_mail(std::move(packet));
}
