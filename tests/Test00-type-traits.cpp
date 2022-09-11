/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include <DynamicFlow/TypeTraits.h>

#include "CheckCPlusPlus20.h"

namespace {
class ComparableByOperator {
public:
  ComparableByOperator() = default;
};

bool operator==(const ComparableByOperator &, const ComparableByOperator &) {
  return true;
}
} // namespace

TEST(TypeTraits, HasComparisonOperator) {
  CHECK_CPLUSPLUS_20

  EXPECT_TRUE(flw::HasComparisonOperator<int>::value);

  class ComparableByMethod {
  public:
    ComparableByMethod() = default;

    bool operator==(const ComparableByMethod &) const { return true; }
  };
  EXPECT_TRUE(flw::HasComparisonOperator<ComparableByMethod>::value);

  EXPECT_TRUE(flw::HasComparisonOperator<ComparableByOperator>::value);

  class UnComparable {
  public:
    UnComparable() = default;
  };
  EXPECT_FALSE(flw::HasComparisonOperator<UnComparable>::value);
}

namespace {
class Printable {
public:
  Printable() = default;
};

std::ostream &operator<<(std::ostream &s, const Printable &) {
  s << "Printable";
  return s;
}
} // namespace

TEST(TypeTraits, CanBeSerialized) {
  CHECK_CPLUSPLUS_20

  EXPECT_EQ(flw::to_string(0), "0");

  EXPECT_EQ(flw::to_string(Printable{}), "Printable");

  class UnPrintable {
  public:
    UnPrintable() = default;
  };
  EXPECT_FALSE(flw::to_string(UnPrintable{}));
}
