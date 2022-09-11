#pragma once

#include "Polygon.h"

#include <Flexible-GJK-and-EPA/shape/TransformDecorator.h>

#include <DynamicFlow/Network.h>

#include <list>

namespace flw::sample {
class FlowHandler {
public:
  Flow flow;

  FlowHandler() = default;

  struct PolygonHandler {
    SourceHandler<float> angle;
    SourceHandler<Point2D> center;
  };
  PolygonHandler addPolygon(Polygon &&polygon, const std::string &name);

  void finalizeFlowCreation(const std::string &outputFileName);

private:
  struct Polygon_ {
    SourceHandler<Polygon> shape;
    NodeHandler<flx::shape::TransformDecorator> decorator;
  };
  std::list<Polygon_> polygons_;
};
} // namespace flw::sample
