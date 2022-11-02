// /**
//  * Author:    Andrea Casalino
//  * Created:   10.09.2021
//  *
//  * report any bug to andrecasa91@gmail.com.
//  **/
#include <gtest/gtest.h>

#include <DynamicFlow/Network.h>

namespace {
template <std::size_t ExpectedIncrement, typename Subject>
class IncrementGuard {
public:
  IncrementGuard(Subject &subject)
      : to_check(subject), initial_size(subject.size()){};

  ~IncrementGuard() {
    EXPECT_EQ(to_check.size(), initial_size + ExpectedIncrement);
  }

private:
  Subject &to_check;
  const std::size_t initial_size;
};

template <std::size_t ExpectedIncrement, typename Subject>
IncrementGuard<ExpectedIncrement, Subject>
make_increment_guard(Subject &subject) {
  return IncrementGuard<ExpectedIncrement, Subject>{subject};
}

template <typename Subject> using UnchangedGuard = IncrementGuard<0, Subject>;

template <typename Subject>
UnchangedGuard<Subject> make_unchanged_guard(Subject &subject) {
  return UnchangedGuard<Subject>{subject};
}

class HandlerMakerTestable : public flw::detail::HandlerMaker,
                             public ::testing::Test {
public:
  HandlerMakerTestable() = default;

  template <typename T> flw::SourceHandler<T> makeTestSource() {
    auto all_source_guard = make_increment_guard<1>(sources_all_);
    auto labeled_source_guard = make_unchanged_guard(sources_labeled_);

    return this->makeSource<T>(std::nullopt);
  }

  template <typename T, typename... Deps, typename EvalPredicate>
  flw::NodeHandler<T> makeTestNode(EvalPredicate &&predicate,
                                   const flw::NodeHandler<Deps> &...deps) {
    auto nodes_all_guard = make_increment_guard<1>(nodes_all_);
    auto nodes_labeled_guard = make_unchanged_guard(nodes_labeled_);

    return this->makeNode<T, Deps...>(std::forward<EvalPredicate>(predicate),
                                      std::nullopt, deps...);
  }

  std::list<flw::NodePtr>::const_iterator
  findNode(const flw::Value &to_find) const {
    return std::find_if(this->nodes_all_.begin(), this->nodes_all_.end(),
                        [&to_find](const flw::NodePtr &n) {
                          return n->valueUntyped() == &to_find;
                        });
  }

  flw::Values findDependencies(const flw::Value &to_find) const {
    return (*this->findNode(to_find))->dependencies();
  }
};
} // namespace

TEST_F(HandlerMakerTestable, OneDependency_OneLevel_creation) {
  auto source = this->makeTestSource<int>();

  auto node =
      this->makeTestNode<int, int>([](const int &) { return 0; }, source);

  EXPECT_EQ(this->findDependencies(node.getValueUntyped()),
            flw::Values{&source.getValueUntyped()});
}

TEST_F(HandlerMakerTestable, FourDependency_OneLevel_creation) {
  auto source1 = this->makeTestSource<int>();
  auto source2 = this->makeTestSource<std::string>();
  auto source3 = this->makeTestSource<int>();
  auto source4 = this->makeTestSource<std::string>();

  auto node = this->makeTestNode<int, int, std::string, int, std::string>(
      [](const int &, const std::string &, const int &, const std::string &) {
        return 0;
      },
      source1, source2, source3, source4);

  const auto node_deps = this->findDependencies(node.getValueUntyped());
  flw::Values node_deps_expected;
  node_deps_expected.emplace(&source1.getValueUntyped());
  node_deps_expected.emplace(&source2.getValueUntyped());
  node_deps_expected.emplace(&source3.getValueUntyped());
  node_deps_expected.emplace(&source4.getValueUntyped());
  EXPECT_EQ(node_deps, node_deps_expected);
}

TEST_F(HandlerMakerTestable, TwoDependencies_TwoLevel_creation) {
  auto source_int = this->makeTestSource<int>();
  auto source_string = this->makeTestSource<std::string>();

  auto node1 = this->makeTestNode<int, int, std::string>(
      [](const int &, const std::string &) { return 0; }, source_int,
      source_string);

  {
    const auto node_deps = this->findDependencies(node1.getValueUntyped());
    flw::Values node_deps_expected;
    node_deps_expected.emplace(&source_int.getValueUntyped());
    node_deps_expected.emplace(&source_string.getValueUntyped());
    EXPECT_EQ(node_deps, node_deps_expected);
  }

  auto node2 = this->makeTestNode<int, int, std::string>(
      [](const int &, const std::string &) { return 0; }, node1, source_string);

  {
    const auto node_deps = this->findDependencies(node2.getValueUntyped());
    flw::Values node_deps_expected;
    node_deps_expected.emplace(&node1.getValueUntyped());
    node_deps_expected.emplace(&source_string.getValueUntyped());
    EXPECT_EQ(node_deps, node_deps_expected);
  }
}

TEST(Flow, SourceLabeled_creation) {
  flw::Flow flow;

  const std::string source0_name = "source0";
  auto source0 = flow.makeSource<int>(source0_name);

  const std::string source1_name = "source1";
  auto source1 = flow.makeSource<int>(source1_name);

  EXPECT_EQ(&flow.findSource<int>(source0_name).getValueUntyped(),
            &source0.getValueUntyped());
  EXPECT_THROW(flow.findSource<float>(source0_name), flw::Error);
  EXPECT_THROW(flow.findSource<int>("foo"), flw::Error);
  EXPECT_EQ(&flow.findSource<int>(source1_name).getValueUntyped(),
            &source1.getValueUntyped());
}

TEST(Flow, NodeLabeled_creation) {
  flw::Flow flow;

  auto source = flow.makeSource<int>();

  const std::string node0_name = "node0";
  auto node0 = flow.makeNode<int, int>([](const int &) { return 0; },
                                       node0_name, source);

  EXPECT_EQ(&flow.findNode<int>(node0_name).getValueUntyped(),
            &node0.getValueUntyped());
  EXPECT_THROW(flow.findNode<float>(node0_name), flw::Error);
  EXPECT_THROW(flow.findNode<int>("foo"), flw::Error);
}
