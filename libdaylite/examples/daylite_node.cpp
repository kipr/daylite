#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <bson.h>

#include "daylite/node.hpp"
#include "daylite/spinner.hpp"
#include <bson_bind/option.hpp>

using namespace daylite;
using namespace std;

void print_bson(const bson_t *const msg)
{
  size_t len;
  char *str;
  str = bson_as_json (msg, &len);
  cout << str << endl;
  bson_free (str);
}

struct test {
  int32_t basic;
  bson_bind::option<std::vector<std::string>> string_arr;
  bson_t *bind() {
    bson_t *ret = bson_new();
    bson_t *arr;
    uint32_t i = 0;
    bson_append_int32(ret, "basic", -1, basic);
    if(string_arr.some()) {
    arr = bson_new();
    i = 0;
    for(vector<std::string>::const_iterator it = string_arr.unwrap().begin();
        it != string_arr.unwrap().end(); ++it, ++i)
      bson_append_utf8(arr, std::to_string(i).c_str(), -1, (*it).c_str(), -1);
    bson_append_array(ret, "string_arr", -1, arr);
    bson_destroy(arr);
    }
    return ret;
  }
  static test unbind(const bson_t *const bson) {
    bson_iter_t it;
    bson_iter_t itt;
    uint32_t i = 0;
    bool found;
    const bson_value_t *v;
    bson_t *arr;
    bson_t *d;
    test ret;
    found = bson_iter_init_find(&it, bson, "basic");
    if(!found) throw std::invalid_argument("required key basic not found in bson document");
    else {
      v = bson_iter_value(&it);
      if(v->value_type != BSON_TYPE_INT32) throw std::invalid_argument("key basic has the wrong type");
      ret.basic = v->value.v_int32;
    }
    found = bson_iter_init_find(&it, bson, "string_arr");
    if(found) {
      v = bson_iter_value(&it);
      if(v->value_type != BSON_TYPE_ARRAY) throw std::invalid_argument("key string_arr has the wrong type");
      arr = bson_new_from_data(v->value.v_doc.data, v->value.v_doc.data_len);
      i = 0;
      ret.string_arr = bson_bind::some<std::vector<std::string> >(std::vector<std::string>());
      for(;; ++i) {
        if(!bson_iter_init_find(&itt, arr, std::to_string(i).c_str())) break;
        v = bson_iter_value(&itt);
        if(v->value_type != BSON_TYPE_UTF8) throw std::invalid_argument("key string_arr child has the wrong type");
        std::string tmp;
        tmp = std::string(v->value.v_utf8.str, v->value.v_utf8.len);
        ret.string_arr.unwrap().push_back(tmp);
      }
      bson_destroy(arr);
    }
    return ret;
  }
};

int main(int argc, char *argv[])
{
  auto me = node::create_node("node");
  if(!me)
  {
    std::cerr << "could not create the node" << std::endl;
    return 1;
  }

  bool ret = me->join_daylite("127.0.0.1", 8374);
  if(!ret)
  {
    std::cerr << "Could not connect to the daylite gateway" << std::endl;
    return 1;
  }

  auto pub = me->advertise("/test");
  if(!pub)
  {
    std::cerr << "Could not advertise to topic /test" << std::endl;
    return 1;
  }
  
  auto sub = me->subscribe("/test", [](const bson_t *msg, void *)
    {
      std::cout << "Got packet" << std::endl;
      test t = test::unbind(msg);
      std::cout << "t.basic = " << t.basic << endl;
      for(auto s : t.string_arr.unwrap())
      {
        std::cout << s << std::endl;
      }
    });

  for(;;)
  {
    test t;
    t.basic = 3;
    t.string_arr = bson_bind::some(vector<string>({"hello", "world!"}));
    pub->publish(t.bind());

    std::this_thread::sleep_for(std::chrono::seconds(1));
    spinner::spin_once();
  }
  
  return 0;
}