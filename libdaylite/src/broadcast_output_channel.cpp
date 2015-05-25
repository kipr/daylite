#include "broadcast_output_channel.hpp"

using namespace daylite;
using namespace std;

broadcast_output_channel::broadcast_output_channel()
{
}

broadcast_output_channel::~broadcast_output_channel()
{
}

void_result broadcast_output_channel::add_output_channel(output_channel *const channel, const void *const assoc)
{
  if(contains_output_channel(assoc)) return failure("Already contains given output_channel"); 
  _channels.push_back(make_tuple(channel, assoc));
  return success();
}

void_result broadcast_output_channel::remove_output_channel(const void *const assoc)
{
  for(auto it = _channels.begin(); it != _channels.end(); ++it)
  {
    if(get<1>(*it) != assoc) continue;
    _channels.erase(it);
    return success();
  }
  return failure("No such output_channel found");
}

bool broadcast_output_channel::contains_output_channel(const void *const assoc)
{
  for(const auto &c : _channels)
  {
    if(get<1>(c) != assoc) continue;
    return true;
  }
  return false;
}

void_result broadcast_output_channel::write(const packet &p)
{
  for(const auto &channel : _channels)
  {
    const void_result s = get<0>(channel)->write(p);
    if(!s) return s;
  }
  return success();
}