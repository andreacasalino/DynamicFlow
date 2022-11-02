#pragma once

#include <Flexible-GJK-and-EPA/shape/PointCloud.h>

#include <array>
#include <memory>
#include <vector>

namespace flw::sample {
float to_rad(float angle);

using Point3D = std::array<float, 3>;
using Points = std::vector<Point3D>;

using Point2D = std::array<float, 2>;

namespace detail {
class PointsStorer {
public:
  virtual ~PointsStorer() = default;

protected:
  PointsStorer(std::shared_ptr<const Points> vertices) : vertices_(vertices) {}

  std::shared_ptr<const Points> vertices_;
};
} // namespace detail

class Polygon : private detail::PointsStorer,
                public flx::shape::PointCloud<Points::const_iterator> {
public:
  Polygon(float ray, std::size_t vertices);
  Polygon(const Polygon &o);
  Polygon(Polygon &&o);

  std::vector<Point2D> getPoints() const;

private:
  Polygon(std::shared_ptr<const Points> vertices);
};
} // namespace flw::sample
