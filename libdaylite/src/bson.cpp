#include <daylite/bson.hpp>
#include <bson.h>

using namespace daylite;
using namespace std;

bson::bson(bson_t *const raw, bool owns)
  : _raw(owns ? raw : bson_copy(raw), bson_destroy)
{
}

bson::bson(const bson_t *const raw)
  : _raw(bson_copy(raw), bson_destroy)
{
}

bson::bson()
{
}

bson::bson(const bson &rhs)
  : _raw(rhs._raw)
{
}

bson::~bson()
{
}

bson &bson::operator =(const bson &rhs)
{
  _raw = rhs._raw;
  return *this;
}
