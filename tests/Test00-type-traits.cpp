/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include <DynamicFlow/TypeTraits.h>

namespace flw::test {
class ComparableByMethod {
public:
  ComparableByMethod() = default;

  bool operator==(const ComparableByMethod &) const { return true; }
};

class ComparableByFunction {
public:
  ComparableByFunction() = default;
};

bool operator==(const ComparableByFunction &a, const ComparableByFunction &b) {
  return true;
}

class UnComparable {
public:
  UnComparable() = default;
};

template <typename T> bool hasComparisonOperator() {
  if constexpr (flw::HasComparisonOperator<T>) {
    return true;
  } else {
    return false;
  }
}

TEST(TypeTraitsTest, HasComparisonOperator) {
  EXPECT_TRUE(hasComparisonOperator<int>());
  EXPECT_TRUE(hasComparisonOperator<ComparableByMethod>());
  EXPECT_TRUE(hasComparisonOperator<ComparableByFunction>());

  EXPECT_FALSE(hasComparisonOperator<UnComparable>());
}

class PrintableWithStream {
public:
  PrintableWithStream() = default;
};

std::ostream &operator<<(std::ostream &s, const PrintableWithStream &) {
  return s;
}

class UnPrintable {
public:
  UnPrintable() = default;
};

template <typename T> bool hasStreamOperator() {
  if constexpr (flw::HasStreamOperator<T>) {
    return true;
  } else {
    return false;
  }
}

TEST(TypeTraitsTest, HasStreamOperator) {
  EXPECT_TRUE(hasStreamOperator<int>());
  EXPECT_TRUE(hasStreamOperator<PrintableWithStream>());

  EXPECT_FALSE(hasStreamOperator<UnPrintable>());
}

template <typename T> bool canBeStringConvertible() {
  if constexpr (flw::CanBeStringConvertible<T>) {
    return true;
  } else {
    return false;
  }
}

TEST(TypeTraitsTest, CanBeStringConvertible) {
  EXPECT_TRUE(canBeStringConvertible<int>());

  EXPECT_FALSE(canBeStringConvertible<UnPrintable>());
}
} // namespace flw::test
