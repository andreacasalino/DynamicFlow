/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/Network.h>

namespace flw {
// dot is a language for describing graph, which is also used by graphviz, refer
// to: https://graphviz.org/doc/info/lang.html
//
// dot files can be also converted to svg, png or pdf. The easiest way to do
// this is to use Python graphviz, refe to: https://pypi.org/project/graphviz/
//
// alterantively, you can copy paste the dot file content into this online
// application: https://dreampuf.github.io/GraphvizOnline/
// that will render the graph for you
std::string to_dot(const FlowSnapshot &structure);

void to_dot(const std::string &filename, const FlowSnapshot &structure);
} // namespace flw

#ifdef DYNAMIC_FLOW_ENABLE_JSON_EXPORT
#include <nlohmann/json.hpp>

namespace flw {
void to_json(nlohmann::json &recipient, const FlowSnapshot &structure);

void to_json(const std::string &filename, const FlowSnapshot &structure);
} // namespace flw
#endif
