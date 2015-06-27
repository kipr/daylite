#include <cstring>

#include "console.hpp"
#include "packet.hpp"

using namespace daylite;
using namespace std;

packet::packet(const bson_t *packed_msg)
  : _topic("--")
  , _msg(nullptr)
{
  bson_iter_t it;
  if(bson_iter_init(&it, packed_msg))
  {
    // get the topic from the bson message
    if(bson_iter_find(&it, "topic"))
    {
      const bson_value_t *val = bson_iter_value(&it);
      if(val->value_type == BSON_TYPE_UTF8) _topic = topic(val->value.v_utf8.str);
      else DAYLITE_ERROR_STREAM("'topic' has value-type " << val->value_type << ", " << BSON_TYPE_UTF8 << " expected");
    }
    else
    {
      DAYLITE_ERROR_STREAM("Couldn't find key 'topic' in BSON message");
    }

    // get the msg from the bson message
    if(bson_iter_find(&it, "msg"))
    {
      const bson_value_t *val = bson_iter_value(&it);
      if(val->value_type == BSON_TYPE_DOCUMENT) _msg = bson_new_from_data(val->value.v_doc.data, val->value.v_doc.data_len);
      else DAYLITE_ERROR_STREAM("'topic' has value-type " << val->value_type << ", " << BSON_TYPE_DOCUMENT << " expected");
    }
    else
    {
      DAYLITE_ERROR_STREAM("Couldn't find key 'msg' in BSON message");
    }
  }
  else
  {
    DAYLITE_ERROR_STREAM("Couldn't create the BSON iterator");
  }
}

packet::packet(const topic &t, const bson_t *raw_msg)
  : _topic(move(t))
  , _msg(bson_new())
{
  BSON_APPEND_UTF8(_msg, "topic", _topic.name().c_str());
  if(raw_msg) BSON_APPEND_DOCUMENT(_msg, "msg", raw_msg);
  else BSON_APPEND_UNDEFINED(_msg, "msg");
}

packet::packet(const packet &rhs)
  : _topic(rhs._topic)
  , _msg(rhs._msg ? bson_copy(rhs._msg) : nullptr)
{
}

packet::packet(packet &&rhs)
  : _topic(move(rhs._topic))
  , _msg(rhs._msg)
{
  rhs._msg = nullptr;
}

packet::~packet()
{
  if(_msg) bson_destroy(_msg);
}

packet &packet::operator =(packet &&rhs)
{
  if(_msg) bson_destroy(_msg);

  _topic = move(rhs._topic);
  _msg = rhs._msg;

  rhs._msg = nullptr;

  return *this;
}

packet &packet::operator =(const packet &rhs)
{
  if(_msg) bson_destroy(_msg);
  _topic = rhs._topic;
  _msg = rhs._msg ? bson_copy(rhs._msg) : nullptr;

  return *this;
}
