#include "CollisionDetectionFlow.h"
#include "ResultsAccumulator.h"

#include <RunScript.h>

#include <algorithm>
#include <fstream>
#include <math.h>
#include <optional>

namespace flw::sample {
FlowHandler::PolygonHandler FlowHandler::addPolygon(Polygon &&polygon,
                                                    const std::string &name) {
  {
    if (table_.contains(name)) {
      throw Error::make<0>("A polygon named ", name,
                           " is already part of the network");
    }
  }

  auto angle = flow.makeSource<float>(0, name + "-angle");
  auto center = flow.makeSource<Point2D>(Point2D{0, 0}, name + "-center");

  auto polygon_source =
      flow.makeSource<Polygon>(std::forward<Polygon>(polygon), name);

  auto polygon_transformation =
      flow.makeNode<flx::shape::Transformation, float, Point2D>(
          [](const float &angle, const Point2D &center) {
            return flx::shape::Transformation{
                hull::Coordinate{center[0], center[1]},
                flx::shape::RotationXYZ{0, 0, angle}};
          },
          angle, center, name + "-roto-traslation");

  auto polygon_with_transformation =
      flow.makeNode<flx::shape::TransformDecorator, Polygon,
                    flx::shape::Transformation>(
          [](const Polygon &polygon, const flx::shape::Transformation &trsf) {
            return flx::shape::TransformDecorator{
                std::make_unique<Polygon>(polygon), trsf};
          },
          polygon_source, polygon_transformation, name + "-transformed");

  polygons_.emplace_front(
      PolygonWithPosition{name, polygon_source, polygon_with_transformation});
  table_.emplace(polygons_.front().label.data(), polygons_.begin());
  return PolygonHandler{angle, center};
}

namespace {
void print(const nlohmann::json &subject) {
  static std::size_t PRINT_COUNTER = 0;
  std::ostringstream filename_complete;
  filename_complete << "Scenario" << '_' << ++PRINT_COUNTER << ".json";
  std::filesystem::path path_tot = LogDir::get() / filename_complete.str();
  std::ofstream stream(path_tot);
  if (!stream.is_open()) {
    throw Error::make<' '>(path_tot, ": invalid log path");
  }
  stream << subject.dump();
}
} // namespace

void FlowHandler::finalizeFlowCreation() {
  if (polygons_.empty()) {
    throw Error{"At least one polygon should be provided"};
  }

  struct Check {
    Handler<flx::QueryResult> query;
    std::string name_a;
    HandlerSource<Polygon> source_a;
    std::string name_b;
    HandlerSource<Polygon> source_b;
  };
  std::vector<Check> checks;
  for (auto a = polygons_.begin(); a != polygons_.end(); ++a) {
    auto b = a;
    ++b;
    for (; b != polygons_.end(); ++b) {
      std::ostringstream label;
      label << a->label << "-vs-" << b->label << "-gjk-result";
      auto gjk_result =
          flow.makeNode<flx::QueryResult, flx::shape::TransformDecorator,
                        flx::shape::TransformDecorator>(
              flx::get_closest_points_or_penetration_info, a->decorator,
              b->decorator, label.str());
      checks.emplace_back(
          Check{gjk_result, a->label, a->shape, b->label, b->shape});
    }
  }

  using Handler = Handler<ResultsAccumulator>;
  std::optional<Handler> accumulator;
  for (const auto &polygon : polygons_) {
    const auto &name = polygon.label;
    if (!accumulator.has_value()) {
      accumulator.emplace(
          flow.makeNode<ResultsAccumulator, flx::shape::TransformDecorator>(
              [name](const flx::shape::TransformDecorator &res) {
                ResultsAccumulator result;
                result.add(name, res);
                return result;
              },
              polygon.decorator, "JSON-" + name));
    } else {
      auto &prev = accumulator.value();
      accumulator.emplace(
          flow.makeNode<ResultsAccumulator, flx::shape::TransformDecorator,
                        ResultsAccumulator>(
              [name](const flx::shape::TransformDecorator &res,
                     const ResultsAccumulator &prev) {
                ResultsAccumulator result{prev};
                result.add(name, res);
                return result;
              },
              polygon.decorator, prev, "JSON-" + name));
    }
  }
  for (const auto &check : checks) {
    const auto &name_a = check.name_a;
    const auto &name_b = check.name_b;
    std::ostringstream label;
    label << "JSON-" << name_a << "-vs-" << name_b << "-gjk-result";
    flw::ValueCallBacks<ResultsAccumulator> cb;
    if (&checks.back() == &check) {
      // register dump callback to last result
      cb.addOnValue(
          [](const ResultsAccumulator &results) { print(results.dump()); });
    }
    auto &prev = accumulator.value();
    accumulator.emplace(
        flow.makeNodeWithErrorsCB<ResultsAccumulator, flx::QueryResult,
                                  ResultsAccumulator>(
            [name_a, name_b](const flx::QueryResult &res,
                             const ResultsAccumulator &prev) {
              ResultsAccumulator result{prev};
              result.add(name_a, name_b, res);
              return result;
            },
            check.query, prev, std::move(cb), label.str()));
  }
}
} // namespace flw::sample
