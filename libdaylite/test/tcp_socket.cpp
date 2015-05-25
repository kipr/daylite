#include "gtest/gtest.h"
#include "daylite/tcp_socket.hpp"
#include <unistd.h>

using namespace daylite;

TEST(tcp_socket, server_bind)
{
  tcp_socket server;
  EXPECT_TRUE(server.open());
  socket_address addr(8372);
  EXPECT_TRUE(server.bind(addr));
  EXPECT_TRUE(server.listen(3));
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}