#pragma once

#include <Flexible-GJK-and-EPA/GjkEpa.h>
#include <Flexible-GJK-and-EPA/shape/TransformDecorator.h>
#include <Polygon.h>

#include <nlohmann/json.hpp>

#include <memory>
#include <unordered_map>

namespace flw::sample {
class ResultsAccumulator {
public:
  ResultsAccumulator();

  struct Results {
    std::unordered_map<std::string, nlohmann::json> serialized_polygons;

    struct ClosestPair {
      std::string nameA;
      std::string nameB;
      nlohmann::json serializedPair; // null represents a collision
    };
    std::vector<ClosestPair> serialized_closest_pairs;
  };

  void add(const std::string &name,
           const flx::shape::TransformDecorator &subject);

  void add(const std::string &nameA, const std::string &nameB,
           const flx::QueryResult &subject);

  nlohmann::json dump() const;

protected:
  std::shared_ptr<Results> results_;
};
} // namespace flw::sample
