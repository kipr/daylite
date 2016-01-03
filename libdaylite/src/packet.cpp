#include <cstring>

#include "console.hpp"
#include "packet.hpp"

using namespace daylite;
using namespace std;

packet::packet()
  : _null(true)
{
}

packet::packet(const bson &packed_msg)
  : _null(false)
  , _packed(packed_msg)
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
    bson m(bson_new_from_data(val->value.v_doc.data, val->value.v_doc.data_len));
    _meta = meta::unbind(m);
  }
  else DAYLITE_ERROR_STREAM("'meta' has value-type " << val->value_type << ", " << BSON_TYPE_DOCUMENT << " expected");

  // get the msg from the bson message
  if(!bson_iter_find(&it, "msg"))
  {
    DAYLITE_ERROR_STREAM("Couldn't find key 'msg' in BSON message");
    return;
  }
  
  val = bson_iter_value(&it);
  if(val->value_type == BSON_TYPE_DOCUMENT) _msg = bson(bson_new_from_data(val->value.v_doc.data, val->value.v_doc.data_len));
  else DAYLITE_ERROR_STREAM("'topic' has value-type " << val->value_type << ", " << BSON_TYPE_DOCUMENT << " expected");
}

packet::packet(const class topic &t, const network_time &stamp, const bson &raw_msg, bool autobuild)
  : _null(false)
  , _msg(raw_msg)
  , _packed()
{
  _meta.topic = t.name();
  _meta.stamp = stamp;
  _meta.droppable = false;
  if(autobuild) build();
}

packet::packet(const packet &rhs)
  : _null(rhs._null)
  , _meta(rhs._meta)
  , _msg(rhs._msg)
  , _packed(rhs._packed)
{
}

packet::packet(packet &&rhs)
  : _null(rhs._null)
  , _meta(move(rhs._meta))
  , _msg(rhs._msg)
  , _packed(rhs._packed)
{
  rhs._msg = bson();
  rhs._packed = bson();
}

packet::~packet()
{
}

void packet::build()
{
  if(_null) return;
  
  auto p = bson_new();
  
  auto m = bson(_meta.bind());
  BSON_APPEND_DOCUMENT(p, "meta", m);
  if(_msg) BSON_APPEND_DOCUMENT(p, "msg", _msg);  
  else BSON_APPEND_UNDEFINED(p, "msg");
  
  _packed = bson(p);
}

packet &packet::operator =(packet &&rhs)
{
  _null = rhs._null;
  _meta = move(rhs._meta);
  _msg = move(rhs._msg);
  _packed = move(rhs._packed);
  return *this;
}

packet &packet::operator =(const packet &rhs)
{
  _null = rhs._null;
  _meta = rhs._meta;
  _msg = rhs._msg;
  _packed = rhs._packed;

  return *this;
}
