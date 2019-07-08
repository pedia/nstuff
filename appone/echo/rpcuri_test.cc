#include "rpcuri.h"

#include "gtest/gtest.h"

using namespace arch;

TEST(Rpcuri, String) {
  RpcUri result;
  bool flag = false;

  flag = RpcUri::Parse("http://localhost:80/foo.html?&q=1:2:3", &result);
  EXPECT_TRUE(flag);

  flag = RpcUri::Parse("https://localhost:80/foo.html?&q=1", &result);
  EXPECT_TRUE(flag);

  flag = RpcUri::Parse("localhost/foo", &result);
  EXPECT_TRUE(flag);

  flag = RpcUri::Parse("https://localhost/foo", &result);
  EXPECT_TRUE(flag);

  flag = RpcUri::Parse("localhost:8080", &result);
  EXPECT_TRUE(flag);

  flag = RpcUri::Parse("localhost?&foo=1", &result);
  EXPECT_TRUE(flag);

  flag = RpcUri::Parse("localhost?&foo=1:2:3", &result);
  EXPECT_TRUE(flag);
}

TEST(Rpcuri, CreateChannelTest) {}

// int main(int argc, char *argv[]) {
//   testing::InitGoogleTest(&argc, argv);
//   int r = RUN_ALL_TESTS();
//   return r;
// }
