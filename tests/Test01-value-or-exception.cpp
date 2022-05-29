/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <DynamicFlow/components/ValueOrException.hpp>
#include <gtest/gtest.h>
#include <sstream>

TEST(ValueOrException, nullptr_comparison) {
  EXPECT_EQ(flw::ValueOrException<int>(), nullptr);
  EXPECT_EQ(nullptr, flw::ValueOrException<int>());

  flw::ValueOrException<int> value;
  value.reset(std::make_unique<int>(0));
  EXPECT_FALSE(nullptr == value);
  EXPECT_FALSE(value == nullptr);
}

static const std::string EMPTY_MESSAGE_ERROR = "Empty message";
std::string merge(const std::string &a, const std::string &b) {
  std::stringstream stream;
  stream << a << b;
  return stream.str();
};

class String {
public:
  String(const std::string &mex) {
    if (mex.empty()) {
      throw flw::Error(EMPTY_MESSAGE_ERROR);
    }
    this->mex = mex;
  };

  String(const std::string &part_a, const std::string &part_b)
      : String(merge(part_a, part_b)){};

  inline const std::string &getMex() const { return this->mex; };

private:
  std::string mex;
};
using StringOrExc = flw::ValueOrException<String>;

TEST(ValueOrException, constructors) {
  const std::string expected_message_part_a = "HelloFr";
  const std::string expected_message_part_b = "omString";
  const std::string expected_message =
      merge(expected_message_part_a, expected_message_part_b);

  {
    StringOrExc value(std::make_unique<String>(expected_message));
    EXPECT_FALSE(nullptr == value);
    EXPECT_TRUE(expected_message.compare(value.get()->getMex()) == 0);
  }

  {
    StringOrExc value(std::make_unique<String>(expected_message_part_a,
                                               expected_message_part_b));
    EXPECT_FALSE(nullptr == value);
    EXPECT_TRUE(expected_message.compare(value.get()->getMex()) == 0);
  }
}

TEST(ValueOrException, reset) {
  const std::string expected_message_part_a = "HelloFr";
  const std::string expected_message_part_b = "omString";
  const std::string expected_message =
      merge(expected_message_part_a, expected_message_part_b);

  StringOrExc value(std::make_unique<String>(expected_message_part_a));
  EXPECT_FALSE(nullptr == value);
  EXPECT_TRUE(expected_message_part_a.compare(value.get()->getMex()) == 0);

  value.reset(std::make_unique<String>(expected_message_part_b));
  EXPECT_FALSE(nullptr == value);
  EXPECT_TRUE(expected_message_part_b.compare(value.get()->getMex()) == 0);

  value.reset(std::make_unique<String>(expected_message_part_a,
                                       expected_message_part_b));
  EXPECT_FALSE(nullptr == value);
  EXPECT_TRUE(expected_message.compare(value.get()->getMex()) == 0);

  value.reset();
  EXPECT_TRUE(nullptr == value);
}

TEST(ValueOrException, exception) {
  StringOrExc value;
  try {
    std::unique_ptr<String> temp = std::make_unique<String>("");
  } catch (const flw::Error &e) {
    value.resetException(std::make_exception_ptr(e));
  }
  EXPECT_TRUE(nullptr == value);
  EXPECT_TRUE(value.isException());

  {
    auto exc = value.getException();
    ASSERT_THROW(std::rethrow_exception(exc), flw::Error);
  }

  {
    auto exc = value.getException();
    try {
      std::rethrow_exception(exc);
    } catch (const std::exception &e) {
      EXPECT_TRUE(std::string(e.what()).compare(EMPTY_MESSAGE_ERROR) == 0);
    }
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
