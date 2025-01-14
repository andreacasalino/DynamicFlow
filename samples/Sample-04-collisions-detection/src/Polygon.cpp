#include "Polygon.h"

#include <math.h>

namespace flw::sample {
namespace {
static constexpr float PI_GREEK = 3.14159f;
static constexpr float PI_GREEK_2 = 2.f * PI_GREEK;
} // namespace

float to_rad(float angle) { return angle * PI_GREEK / 180.f; }

namespace {
std::shared_ptr<Points> make_vertices(float ray, std::size_t vertices) {
  Points result;
  result.reserve(vertices * 2);
  const float angle_delta = PI_GREEK_2 / static_cast<float>(vertices);
  float angle = 0;
  for (std::size_t k = 0; k < vertices; ++k, angle += angle_delta) {
    float x = ray * cosf(angle);
    float y = ray * sinf(angle);
    result.emplace_back(Point3D{x, y, 0.1f});
    result.emplace_back(Point3D{x, y, -0.1f});
  }
  return std::make_shared<Points>(std::move(result));
}

float product(const Points::const_iterator &element,
              const hull::Coordinate &direction) {
  return element->at(0) * direction.x + element->at(1) * direction.y +
         element->at(2) * direction.z;
}

hull::Coordinate to_coordinate(const Points::const_iterator &to_convert) {
  return hull::Coordinate{to_convert->at(0), to_convert->at(1),
                          to_convert->at(2)};
}
} // namespace

Polygon::Polygon(float ray, std::size_t vertices)
    : Polygon{make_vertices(ray, vertices)} {}

Polygon::Polygon(std::shared_ptr<const Points> vertices)
    : detail::PointsStorer{vertices},
      flx::shape::PointCloud<Points::const_iterator>{
          vertices_->begin(), vertices_->end(), product, to_coordinate} {}

std::vector<Point2D> Polygon::getPoints() const {
  std::vector<Point2D> result;
  for (std::size_t k = 0; k < vertices_->size(); k += 2) {
    const auto &vertex = vertices_->at(k);
    result.push_back(Point2D{vertex[0], vertex[1]});
  }
  return result;
}
} // namespace flw::sample
