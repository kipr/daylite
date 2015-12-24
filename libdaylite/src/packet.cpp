#include <cstring>

#include "console.hpp"
#include "packet.hpp"

using namespace daylite;
using namespace std;

packet::packet(const bson_t *packed_msg)
  : _msg(nullptr)
  , _packed(bson_copy(packed_msg))
{
  bson_iter_t it;
  if(!bson_iter_init(&it, packed_msg))
  {
    DAYLITE_ERROR_STREAM("Couldn't create the BSON iterator");
    return;
  }
  
  // get the meta info from the bson message
  if(!bson_iter_find(&it, "meta"))
  {
    DAYLITE_ERROR_STREAM("Couldn't find key 'meta' in BSON message");
    return;
  }
  const bson_value_t *val = bson_iter_value(&it);
  if(val->value_type == BSON_TYPE_DOCUMENT)
  {
    bson_t *m = bson_new_from_data(val->value.v_doc.data, val->value.v_doc.data_len);
    _meta = meta::unbind(m);
    bson_destroy(m);
  }
  else DAYLITE_ERROR_STREAM("'meta' has value-type " << val->value_type << ", " << BSON_TYPE_DOCUMENT << " expected");

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

packet::packet(const class topic &t, const network_time &stamp, const bson_t *raw_msg)
  : _msg(bson_copy(raw_msg))
  , _packed(nullptr)
{
  _meta.topic = t.name();
  _meta.stamp = stamp;
  build();
}

packet::packet(const packet &rhs)
  : _meta(rhs._meta)
  , _msg(rhs._msg ? bson_copy(rhs._msg) : nullptr)
  , _packed(rhs._packed ? bson_copy(rhs._packed) : nullptr)
{
}

packet::packet(packet &&rhs)
  : _meta(move(rhs._meta))
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

void packet::build()
{
  if(_packed) bson_destroy(_packed);
  _packed = bson_new();
  auto m = _meta.bind();
  BSON_APPEND_DOCUMENT(_packed, "meta", m);
  bson_destroy(m);
  if(_msg) BSON_APPEND_DOCUMENT(_packed, "msg", _msg);
  else BSON_APPEND_UNDEFINED(_packed, "msg");
}

packet &packet::operator =(packet &&rhs)
{
  if(_msg) bson_destroy(_msg);
  if(_packed) bson_destroy(_packed);

  _meta = move(rhs._meta);
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
  _meta = rhs._meta;
  _msg = rhs._msg ? bson_copy(rhs._msg) : nullptr;
  _packed = rhs._packed ? bson_copy(rhs._packed) : nullptr;

  return *this;
}
