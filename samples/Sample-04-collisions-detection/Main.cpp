#include <iostream>

#include <DynamicFlow/Network.hxx>
#include <DynamicFlow/NetworkSerialization.h>

#include "CollisionDetectionFlow.h"
#include <RunScript.h>

int main() {
  //////////////////////////////////////////////////////////////
  // build the collision check network
  flw::sample::FlowHandler flow_handler;
  auto &flow = flow_handler.flow;

  auto &&[triangle_a_orientation, triangle_a_pos] =
      flow_handler.addPolygon(flw::sample::Polygon{1.f, 3}, "triangle_a");
  auto &&[triangle_b_orientation, triangle_b_pos] =
      flow_handler.addPolygon(flw::sample::Polygon{1.f, 3}, "triangle_b");
  auto &&[square_orientation, square_pos] =
      flow_handler.addPolygon(flw::sample::Polygon{1.f, 4}, "square");
  auto &&[circle_orientation, circle_pos] =
      flow_handler.addPolygon(flw::sample::Polygon{1.f, 30}, "almost_circle");
  flow_handler.finalizeFlowCreation();

  // take a snapshot of the network and export it as a .dot file
  flw::Converter<flw::Serialization::DOT>::toFile(
      flw::sample::LogDir::get() / "Flow-Sample-04.dot", flow.snapshot());
  // use python graphviz to render exported .dot file
  flw::sample::RunScript::runDefaultScript("Flow-Sample-04.dot");

  // sets multiple threads for the flow update
  flow.setThreads(3);

  //////////////////////////////////////////////////////////////
  // first scenario -> no collisions at all
  square_orientation.update(flw::sample::to_rad(45.f));
  triangle_a_pos.update(flw::sample::Point2D{-3.f, 0});
  triangle_b_pos.update(flw::sample::Point2D{3.f, 1.5f});
  triangle_b_orientation.update(flw::sample::to_rad(45.f));
  circle_pos.update(flw::sample::Point2D{0, -3.f});
  // recompute proximity queries
  flow.update();
  // show results
  flw::sample::RunScript::runScript(
      std::filesystem::path{SCRIPT_FOLDER} / "ShowResults.py", "file",
      flw::sample::LogDir::get() / "Scenario_1.json");

  //////////////////////////////////////////////////////////////
  // second scenario -> a pair in collision the other no
  triangle_a_pos.update(flw::sample::Point2D{-1.5f, 0});
  circle_pos.update(flw::sample::Point2D{3.f, -0.5f});
  // recompute proximity queries
  //
  // Notice that some results will be cached from previous scenario as not all
  // polygon were moved
  flow.update();
  // show results
  flw::sample::RunScript::runScript(
      std::filesystem::path{SCRIPT_FOLDER} / "ShowResults.py", "file",
      flw::sample::LogDir::get() / "Scenario_2.json");

  return EXIT_SUCCESS;
}
