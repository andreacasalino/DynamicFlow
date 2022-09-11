#include "CollisionDetectionFlow.h"
#include "ResultsAccumulator.h"

#include <algorithm>
#include <fstream>
#include <math.h>

namespace flw::sample {
FlowHandler::PolygonHandler FlowHandler::addPolygon(Polygon &&polygon,
                                                    const std::string &name) {
  {
    auto it = std::find_if(polygons_.begin(), polygons_.end(),
                           [&name](const Polygon_ &element) {
                             return name == element.shape.label().value();
                           });
    if (it != polygons_.end()) {
      throw Error{"A polygon named ", name, " is already part of the network"};
    }
  }

  auto angle = flow.makeSource<float>(name + "-angle");
  angle.update(0.f);
  auto center = flow.makeSource<Point2D>(name + "-center");
  center.update(Point2D{0.f, 0.f});

  auto polygon_source = flow.makeSource<Polygon>(name);
  polygon_source.update(std::forward<Polygon>(polygon));

  auto polygon_transformation =
      flow.makeNode<flx::shape::Transformation, float, Point2D>(
          [](const float &angle, const Point2D &center) {
            return flx::shape::Transformation{
                hull::Coordinate{center[0], center[1]},
                flx::shape::RotationXYZ{0, 0, angle}};
          },
          name + "-roto-traslation", angle, center);

  auto polygon_with_transformation =
      flow.makeNode<flx::shape::TransformDecorator, Polygon,
                    flx::shape::Transformation>(
          [](const Polygon &polygon, const flx::shape::Transformation &trsf) {
            return flx::shape::TransformDecorator{
                std::make_unique<Polygon>(polygon), trsf};
          },
          name + "-transformed", polygon_source, polygon_transformation);

  polygons_.emplace_back(Polygon_{polygon_source, polygon_with_transformation});

  return PolygonHandler{angle, center};
}

namespace {
static std::size_t PRINT_COUNTER = 0;

std::string make_file_name(const std::string &fileName) {
  std::ostringstream result;
  result << fileName << '_' << ++PRINT_COUNTER << ".json";
  return result.str();
}

void print(const std::string &fileName, const nlohmann::json &subject) {
  const auto fileNameWithCounter = make_file_name(fileName);
  std::ofstream stream(fileNameWithCounter);
  if (!stream.is_open()) {
    std::ostringstream error;
    error << fileName << " is an invalid fileName";
    throw std::runtime_error{error.str()};
  }
  stream << subject.dump();
}
} // namespace

void FlowHandler::finalizeFlowCreation(const std::string &outputFileName) {
  if (polygons_.empty()) {
    throw Error{"At least one polygon should be provided"};
  }

  struct Check {
    NodeHandler<flx::QueryResult> query;
    SourceHandler<Polygon> source_a;
    SourceHandler<Polygon> source_b;
  };
  std::list<Check> checks;
  for (auto a = polygons_.begin(); a != polygons_.end(); ++a) {
    auto b = a;
    ++b;
    for (; b != polygons_.end(); ++b) {
      std::ostringstream label;
      label << a->shape.label().value() << "-vs-" << b->shape.label().value()
            << "-gjk-result";
      auto gjk_result =
          flow.makeNode<flx::QueryResult, flx::shape::TransformDecorator,
                        flx::shape::TransformDecorator>(
              flx::get_closest_points_or_penetration_info, label.str(),
              a->decorator, b->decorator);
      checks.emplace_back(Check{gjk_result, a->shape, b->shape});
    }
  }

  using Handler = NodeHandler<ResultsAccumulator>;
  std::unique_ptr<Handler> accumulator;
  for (const auto &polygon : polygons_) {
    const std::string name = polygon.shape.label().value();
    if (nullptr == accumulator) {
      accumulator = std::make_unique<Handler>(
          flow.makeNode<ResultsAccumulator, flx::shape::TransformDecorator>(
              [name](const flx::shape::TransformDecorator &res) {
                ResultsAccumulator result;
                result.add(name, res);
                return result;
              },
              "JSON-" + name, polygon.decorator));
    } else {
      accumulator = std::make_unique<Handler>(
          flow.makeNode<ResultsAccumulator, flx::shape::TransformDecorator,
                        ResultsAccumulator>(
              [name](const flx::shape::TransformDecorator &res,
                     const ResultsAccumulator &prev) {
                ResultsAccumulator result{prev};
                result.add(name, res);
                return result;
              },
              "JSON-" + name, polygon.decorator, *accumulator));
    }
  }
  for (const auto &check : checks) {
    const auto name_a = check.source_a.label().value();
    const auto name_b = check.source_b.label().value();
    std::ostringstream label;
    label << "JSON-" << name_a << "-vs-" << name_b << "-gjk-result";
    accumulator = std::make_unique<Handler>(
        flow.makeNode<ResultsAccumulator, flx::QueryResult, ResultsAccumulator>(
            [name_a, name_b](const flx::QueryResult &res,
                             const ResultsAccumulator &prev) {
              ResultsAccumulator result{prev};
              result.add(name_a, name_b, res);
              return result;
            },
            label.str(), check.query, *accumulator));
  }

  accumulator->onNewValueCallBack(
      [out = outputFileName](const ResultsAccumulator &results) {
        print(out, results.dump());
      });
}
} // namespace flw::sample
