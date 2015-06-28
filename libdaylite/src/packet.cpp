#include <cstring>

#include "console.hpp"
#include "packet.hpp"

using namespace daylite;
using namespace std;

packet::packet(const bson_t *packed_msg)
  : _topic("--")
  , _msg(nullptr)
  , _packed(bson_copy(packed_msg))
{
  bson_iter_t it;
  if(!bson_iter_init(&it, packed_msg))
  {
    DAYLITE_ERROR_STREAM("Couldn't create the BSON iterator");
    return;
  }
  
  // get the topic from the bson message
  if(!bson_iter_find(&it, "topic"))
  {
    DAYLITE_ERROR_STREAM("Couldn't find key 'topic' in BSON message");
    return;
  }
  const bson_value_t *val = bson_iter_value(&it);
  if(val->value_type == BSON_TYPE_UTF8) _topic = daylite::topic(val->value.v_utf8.str);
  else DAYLITE_ERROR_STREAM("'topic' has value-type " << val->value_type << ", " << BSON_TYPE_UTF8 << " expected");

  // get the msg from the bson message
  if(!bson_iter_find(&it, "msg"))
  {
    DAYLITE_ERROR_STREAM("Couldn't find key 'msg' in BSON message");
    return;
  }
  
  val = bson_iter_value(&it);
  if(val->value_type == BSON_TYPE_DOCUMENT) _msg = bson_new_from_data(val->value.v_doc.data, val->value.v_doc.data_len);
  else DAYLITE_ERROR_STREAM("'topic' has value-type " << val->value_type << ", " << BSON_TYPE_DOCUMENT << " expected");
}

packet::packet(const class topic &t, const bson_t *raw_msg)
  : _topic(move(t))
  , _msg(bson_copy(raw_msg))
  , _packed(bson_new())
{
  BSON_APPEND_UTF8(_packed, "topic", _topic.name().c_str());
  if(raw_msg) BSON_APPEND_DOCUMENT(_packed, "msg", raw_msg);
  else BSON_APPEND_UNDEFINED(_packed, "msg");
}

packet::packet(const packet &rhs)
  : _topic(rhs._topic)
  , _msg(rhs._msg ? bson_copy(rhs._msg) : nullptr)
  , _packed(rhs._packed ? bson_copy(rhs._packed) : nullptr)
{
}

packet::packet(packet &&rhs)
  : _topic(move(rhs._topic))
  , _msg(rhs._msg)
  , _packed(rhs._packed)
{
  rhs._msg = nullptr;
  rhs._packed = nullptr;
}

packet::~packet()
{
  if(_msg) bson_destroy(_msg);
  if(_packed) bson_destroy(_packed);
}

packet &packet::operator =(packet &&rhs)
{
  if(_msg) bson_destroy(_msg);
  if(_packed) bson_destroy(_packed);

  _topic = move(rhs._topic);
  _msg = rhs._msg;
  _packed = rhs._packed;

  rhs._msg = nullptr;
  rhs._packed = nullptr;

  return *this;
}

packet &packet::operator =(const packet &rhs)
{
  if(_msg) bson_destroy(_msg);
  if(_packed) bson_destroy(_packed);
  _topic = rhs._topic;
  _msg = rhs._msg ? bson_copy(rhs._msg) : nullptr;
  _packed = rhs._packed ? bson_copy(rhs._packed) : nullptr;

  return *this;
}
