#include "subscriber_impl.hpp"
#include "node_impl.hpp"

using namespace daylite;
using namespace std;

service_impl::service_impl(node_impl *const parent, const daylite::topic &t,
  service::service_callback_t callback)
  : _parent(parent)
  , _t(t)
  , _out(parent->advertise(t.name() + ":out"))
  , _in(parent->subscribe(t.name() + ":in", [this] (const bson &msg, void *)
  {
    _out->publish(_callback(msg));
  }, 0))
{
}