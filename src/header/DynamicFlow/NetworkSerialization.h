/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/Network.hxx>

#ifdef DYNAMIC_FLOW_ENABLE_JSON_EXPORT
#include <nlohmann/json.hpp>
#endif

#include <filesystem>
#include <ostream>
#include <sstream>

namespace flw {

enum class Serialization { DOT, JSON };

template <Serialization> struct Converter {};

// dot is a language for describing graph, which is also used by graphviz,
// refer to: https://graphviz.org/doc/info/lang.html
//
// dot files can be also converted to svg, png or pdf. The easiest way to do
// this is to use Python graphviz, refe to:
// https:// pypi.org/project/graphviz/
//
// alterantively, you can copy paste the dot file content into this
// online application: https://dreampuf.github.io/GraphvizOnline/
// that will render the graph for you
template <> struct Converter<Serialization::DOT> {
  static void toStream(std::ostream &recipient, const FlowSnapshot &structure);

  static std::string toString(const FlowSnapshot &structure) {
    std::stringstream res;
    toStream(res, structure);
    return res.str();
  }

  static void toFile(const std::filesystem::path &filename,
                     const FlowSnapshot &structure);
};

#ifdef DYNAMIC_FLOW_ENABLE_JSON_EXPORT

template <> struct Converter<Serialization::JSON> {
  static void toJson(nlohmann::json &recipient, const FlowSnapshot &structure);

  static void toJsonFile(const std::filesystem::path &filename, const FlowSnapshot &structure);
};

#endif
} // namespace flw
