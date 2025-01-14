#pragma once

#include "Polygon.h"

#include <Flexible-GJK-and-EPA/shape/TransformDecorator.h>

#include <DynamicFlow/Network.hxx>

#include <filesystem>
#include <list>
#include <string_view>
#include <unordered_map>

namespace flw::sample {
class FlowHandler {
public:
  Flow flow;

  FlowHandler() {
    flow.setOnNewNodePolicy(HandlerMaker::OnNewNodePolicy::DEFERRED_UPDATE);
  }

  struct PolygonHandler {
    HandlerSource<float> angle;
    HandlerSource<Point2D> center;
  };
  PolygonHandler addPolygon(Polygon &&polygon, const std::string &name);

  void finalizeFlowCreation();

private:
  struct PolygonWithPosition {
    std::string label;
    HandlerSource<Polygon> shape;
    Handler<flx::shape::TransformDecorator> decorator;
  };
  using Container = std::list<PolygonWithPosition>;
  Container polygons_;
  std::unordered_map<std::string_view, Container::const_iterator> table_;
};
} // namespace flw::sample
