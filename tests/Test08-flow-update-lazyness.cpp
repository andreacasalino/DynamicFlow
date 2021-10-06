/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/Flow.h>
#include <gtest/gtest.h>

class FlowTest : public ::testing::Test, public flw::Flow {
public:
    void SetUp() override {
        auto cloner = [](const auto& in) { return in; };
        auto combiner = [](const auto& in1, const auto& in2) { return in1 + in2; };

        auto S0 = this->makeSource<int>("S0");
        auto S1 = this->makeSource<int>("S1");
        auto S2 = this->makeSource<int>("S2");
        auto S3 = this->makeSource<int>("S3");

        auto M0 = this->makeNode(
            "M0", std::function<int(const int&, const int&)>(combiner), S0, S1);

        auto M1 = this->makeNode(
            "M1", std::function<int(const int&, const int&)>(combiner), S2, S3);

        auto D0 = this->makeNode("D0", std::function<int(const int&)>(cloner), M0);

        auto D1 = this->makeNode(
            "D1", std::function<int(const int&, const int&)>(combiner), M0, M1);

        auto F0 = this->makeNode(
            "F0", std::function<int(const int&, const int&)>(combiner), D0, D1);

        this->updateFlow(S0.getName(), std::make_unique<int>(1), S1.getName(),
            std::make_unique<int>(1), S2.getName(),
            std::make_unique<int>(1), S3.getName(),
            std::make_unique<int>(1));
        this->waitUpdateComplete();
        this->checkValues();
    }

    void checkValues() {
        auto M0 = this->findNode<int, int, int>("M0");
        EXPECT_TRUE(M0.isValue());
        EXPECT_EQ(flw::copyValue(M0), 2);

        auto M1 = this->findNode<int, int, int>("M1");
        EXPECT_TRUE(M1.isValue());
        EXPECT_EQ(flw::copyValue(M1), 2);

        auto D0 = this->findNode<int, int>("D0");
        EXPECT_TRUE(D0.isValue());
        EXPECT_EQ(flw::copyValue(D0), 2);

        auto D1 = this->findNode<int, int, int>("D1");
        EXPECT_TRUE(D1.isValue());
        EXPECT_EQ(flw::copyValue(D1), 4);

        auto F0 = this->findNode<int, int, int>("F0");
        EXPECT_TRUE(F0.isValue());
        EXPECT_EQ(flw::copyValue(F0), 6);
    }
};

TEST_F(FlowTest, update_lazyness_case_0) {
    this->updateFlow("S0", std::make_unique<int>(1), "S1",
        std::make_unique<int>(1));
    this->waitUpdateComplete();

    this->checkValues();

    auto M0 = this->findNode<int, int, int>("M0");
    EXPECT_EQ(M0.getGeneration(), 2);

    auto M1 = this->findNode<int, int, int>("M1");
    EXPECT_EQ(M1.getGeneration(), 1);

    auto D0 = this->findNode<int, int>("D0");
    EXPECT_EQ(D0.getGeneration(), 2);

    auto D1 = this->findNode<int, int, int>("D1");
    EXPECT_EQ(D1.getGeneration(), 2);

    auto F0 = this->findNode<int, int, int>("F0");
    EXPECT_EQ(F0.getGeneration(), 2);
}

TEST_F(FlowTest, update_lazyness_case_1) {
    this->updateFlow("S2", std::make_unique<int>(1), "S3",
        std::make_unique<int>(1));
    this->waitUpdateComplete();

    this->checkValues();

    auto M0 = this->findNode<int, int, int>("M0");
    EXPECT_EQ(M0.getGeneration(), 1);

    auto M1 = this->findNode<int, int, int>("M1");
    EXPECT_EQ(M1.getGeneration(), 2);

    auto D0 = this->findNode<int, int>("D0");
    EXPECT_EQ(D0.getGeneration(), 1);

    auto D1 = this->findNode<int, int, int>("D1");
    EXPECT_EQ(D1.getGeneration(), 2);

    auto F0 = this->findNode<int, int, int>("F0");
    EXPECT_EQ(F0.getGeneration(), 2);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}