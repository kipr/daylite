#include "splat.hpp"

#ifdef SPLAT_ENABLED

#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <sys/mman.h>

using namespace daylite;
using namespace std;

struct splat_data
{
  pthread_mutex_t rw_mutex;
  uint32_t version;
  uint32_t size;
  uint8_t data[0];
};

splat::splat(const uint32_t node_id, const std::string &topic)
  : _node_id(node_id)
  , _topic(topic)
  , _fd(-1)
  , _backing(0)
  , _size(0)
  , _mode(off)
{
}

splat::~splat()
{
  
}

void splat::push(const packet &latest)
{
  assert(_mode == pub);
  const auto &packed = latest.packed();
  
  const uint32_t size = packed->len;
  const uint8_t *data = bson_get_data(packed);
  ++_current_version;
  
  pthread_mutex_lock(&_backing->rw_mutex);
  _backing->version = _current_version;
  _backing->size = size;
  memcpy(_backing->data, data, size);
  pthread_mutex_unlock(&_backing->rw_mutex);
}

void splat::poll()
{
  assert(_mode == sub);
  
  pthread_mutex_lock(&_backing->rw_mutex);
  _current_version = _backing->version;
  
  auto reader = bson_reader_new_from_data(_backing->data, _backing->size);
  _latest = packet(bson_reader_read(reader, 0));
  bson_reader_destroy(reader);
  _latest->set_local_only(true);
  
  pthread_mutex_unlock(&_backing->rw_mutex);
}

bool splat::update_available() const
{
  return _backing->version != _current_version;
}

void_result splat::create(const size_t max_size)
{
  void_result ret = map(max_size, PROT_WRITE);
  if(ret)
  {
    _mode = pub;
    _backing->owner_connected = true;
  }
  return ret;
}

void_result splat::connect(const size_t defined_max_size)
{
  void_result ret = map(defined_max_size, PROT_READ);
  if(ret) _mode = sub;
  return ret;
}

void_result splat::close()
{
  assert(_mode != off);
  _mode = off;
  return unmap(_size);
}

void_result splat::map(const size_t size, const int mode)
{
  stringstream file;
  
  // Remove / from topic name to make a valid file name
  auto fixed_topic = _topic;
  replace(fixed_topic.begin(), fixed_topic.end(), '/', '!');
  
  file << "/tmp/" << _node_id << "-" << fixed_topic << ".splat";
  
  auto filename = file.str();
  _fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
  if(_fd < 0) return failure("Failed to create " + file.str());
  _backing = reinterpret_cast<splat_data *>(mmap(0, size, mode, MAP_SHARED, fd, 0));
  if(reinterpret_cast<void *>(_backing) == MAP_FAILED)
  {
    close(_fd);
    _fd = -1;
    return failure("Failed to memory map file " + filename);
  }
  
  _size = size;
  
  return success();
}

void_result splat::unmap()
{
  if(_fd < 0) return failure("fd is invalid");
  assert(reinterpret_cast<void *>(_backing) != MAP_FAILED && _backing != 0);
  
  if(munmap(reinterpret_cast<void *>(_backing), _size) < 0)
  {
    return failure("Memory un-mapping failed!");
  }
  
  close(_fd);
  _fd = -1;
  _backing = 0;
  _mode = off;
  _size = 0;
  
  return success();
}

#endif