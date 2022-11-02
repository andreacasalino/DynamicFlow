#include "ResultsAccumulator.h"

#include <unordered_set>

namespace flw::sample {
ResultsAccumulator::ResultsAccumulator() {
  results_ = std::make_shared<Results>();
}

ResultsAccumulator::ResultsAccumulator(const ResultsAccumulator &o) {
  results_ = o.results_;
}

namespace {
void to_json(nlohmann::json &recipient, const hull::Coordinate &subject) {
  recipient["x"] = subject.x;
  recipient["y"] = subject.y;
}

void to_json(nlohmann::json &recipient, const Point3D &point) {
  recipient["x"] = point[0];
  recipient["y"] = point[1];
}
} // namespace

void ResultsAccumulator::add(const std::string &name,
                             const flx::shape::TransformDecorator &subject) {
  auto &recipient = results_->serialized_polygons[name];
  const auto *as_polygon = dynamic_cast<const Polygon *>(&subject.getShape());
  const auto trsf = subject.getTransformation();
  auto points = as_polygon->getPoints();
  // apply the rototraslation
  const auto &rot = subject.getTransformation().getRotation();
  const auto &trasl = subject.getTransformation().getTraslation();
  for (auto &point : points) {
    float x = rot[0][0] * point[0] + rot[0][1] * point[1] + trasl.x;
    float y = rot[1][0] * point[0] + rot[1][1] * point[1] + trasl.y;
    point[0] = x;
    point[1] = y;
  }
  nlohmann::to_json(recipient, points);
}

void ResultsAccumulator::add(const std::string &nameA, const std::string &nameB,
                             const flx::QueryResult &subject) {
  auto &recipient =
      results_->serialized_closest_pairs
          .emplace_back(Results::ClosestPair{nameA, nameB, nlohmann::json{}})
          .serializedPair;

  if (subject.is_closest_pair_or_penetration_info) {
    to_json(recipient["diffA"], subject.result.point_in_shape_a);
    to_json(recipient["diffB"], subject.result.point_in_shape_b);
  }
}

namespace {
using Color = std::array<float, 3>;
static const Color RED = {1.f, 0.f, 0.f};
static const Color GREEN = {0.f, 1.f, 0.f};
} // namespace

nlohmann::json ResultsAccumulator::dump() const {
  // apply red colors to polygons in collision and green to others
  // +
  // extract the closest pairs

  nlohmann::json result;
  auto &shapes = result["Shapes"];
  for (const auto &[name, serialization] : results_->serialized_polygons) {
    auto &shape = shapes[name];
    shape["vertices"] = serialization;
    shape["color"] = GREEN;
  }
  auto &lines = result["Lines"];
  lines = nlohmann::json::array();
  for (const auto &pair : results_->serialized_closest_pairs) {
    if (pair.serializedPair.is_null()) {
      // collision: set to red the polygons color
      shapes[pair.nameA]["color"] = RED;
      shapes[pair.nameB]["color"] = RED;
    } else {
      // no collision: put json of the closest pair
      auto &line = lines.emplace_back();
      line = pair.serializedPair;
    }
  }
  return result;
}
} // namespace flw::sample
