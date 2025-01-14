/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include <DynamicFlow/CallBack.h>

struct CallBackTest : ::testing::Test {
    void add() {
        cb.add([this](const std::string& wrd) {
            elements.push_back(wrd);
        });
    }

    std::vector<std::string> elements;
    flw::CallBack<std::string> cb;
};

TEST_F(CallBackTest, empty) {
    cb("foo");
    ASSERT_TRUE(elements.empty());
}

TEST_F(CallBackTest, single_predicate) {
    add();

    cb("foo");
    ASSERT_EQ(elements, std::vector<std::string>{"foo"});
}

TEST_F(CallBackTest, multiple_predicate) {
    add();
    add();
    add();

    cb("foo");
    std::vector<std::string> expected{"foo", "foo", "foo"};
    ASSERT_EQ(elements, expected);
}
