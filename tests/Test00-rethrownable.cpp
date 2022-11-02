/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include <DynamicFlow/Error.h>

namespace {
class Error1 : public flw::Error {
public:
  using flw::Error::Error;
};

class Error2 : public flw::Error {
public:
  using flw::Error::Error;
};
} // namespace

TEST(ReThrownable, Nothing_Throwned) {
  flw::ReThrownable<std::exception, Error1, Error2> thrownable;

  const bool handle_res = thrownable.handle([]() {});
  EXPECT_FALSE(handle_res);
}

TEST(ReThrownable, Error_Throwned) {
  const std::string err_what = "foo";

  {
    flw::ReThrownable<std::exception, Error1, Error2> thrownable;

    const bool handle_res =
        thrownable.handle([&err_what]() { throw Error1{err_what}; });
    EXPECT_TRUE(handle_res);
    EXPECT_THROW(thrownable.reThrow(), Error1);
    try {
      thrownable.reThrow();
    } catch (const Error1 &e) {
      EXPECT_EQ(e.what(), err_what);
    }
  }

  {
    flw::ReThrownable<std::exception, Error1, Error2> thrownable;

    const bool handle_res =
        thrownable.handle([&err_what]() { throw Error2{err_what}; });
    EXPECT_TRUE(handle_res);
    EXPECT_THROW(thrownable.reThrow(), Error2);
    try {
      thrownable.reThrow();
    } catch (const Error2 &e) {
      EXPECT_EQ(e.what(), err_what);
    }
  }
}
