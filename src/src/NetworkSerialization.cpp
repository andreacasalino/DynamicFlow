#include <DynamicFlow/NetworkSerialization.h>

#include <fstream>
#include <string_view>

namespace flw {
namespace {
std::unique_ptr<std::ofstream>
make_stream(const std::filesystem::path &filename) {
  auto res = std::make_unique<std::ofstream>(filename);
  if (!res->is_open()) {
    throw Error::make<' '>(filename, "is not a valid file name");
  }
  return res;
}
} // namespace

void Converter<Serialization::DOT>::toFile(
    const std::filesystem::path &filename, const FlowSnapshot &structure) {
  auto stream = make_stream(filename);
  toStream(*stream, structure);
}

namespace {
struct Format {
  template <typename... ARGS>
  static void apply(std::ostream &recipient, std::string_view frmt,
                    const ARGS &...args) {
    Format tmp{recipient, frmt};
    tmp.apply_(args...);
    recipient << tmp.rest;
  }

private:
  Format(std::ostream &rec, std::string_view frmt)
      : recipient{rec}, rest{frmt} {}

  template <typename ARGF, typename... ARGS>
  void apply_(const ARGF &front, const ARGS &...args) {
    auto next = rest.find("{}");
    if (next == std::string::npos) {
      throw Error{"Invalid format"};
    }
    recipient << std::string_view{rest.data(), next};
    recipient << front;
    rest = std::string_view{rest.data() + next + 2};

    if constexpr (0 < sizeof...(ARGS)) {
      apply_<ARGS...>(args...);
    }
  }

  std::ostream &recipient;
  std::string_view rest;
};

std::string_view node_status(bool engaged) {
  return engaged ? R"(<font color="darkgreen">FILLED</font>)"
                 : R"(<font color="red">UNSET</font>)";
}

const std::string& make_label_name(std::string& label, const ValueSnapshot& el) {
  label.clear();
  if (el.label.empty()) {
    label += "Un-named-";
    label += std::to_string(el.id);
  } else {
    label = el.label;
  }
  return label;
}
} // namespace

void Converter<Serialization::DOT>::toStream(std::ostream &recipient,
                                             const FlowSnapshot &structure) {
  recipient << "digraph DynamicFlow {\n";
  recipient << R"(node [shape=plaintext fontname="Sans serif" fontsize="8"];)"
            << "\n\n";
  std::string label;
  for (const auto &node : structure) {
    make_label_name(label, node);

    Format::apply(recipient, R"({} [ label=<
<table border="1" cellborder="0" cellspacing="1">
  <tr><td align="left"><b>{}</b></td></tr>
  <tr><td align="left">epoch: {}</td></tr>
  <tr><td align="left">status: {}</td></tr>
  <tr><td align="left">value: {}</td></tr>
</table>>];)",
                  node.id, label, node.generation, node_status(node.has_value),
                  node.value_serialization);

    recipient << '\n';
    for (const auto &dep_id : node.dependants) {
      Format::apply(recipient, "{} -> {};\n", node.id, dep_id);
    }
    recipient << '\n';
  }
  recipient << "}\n";
}

#ifdef DYNAMIC_FLOW_ENABLE_JSON_EXPORT
void Converter<Serialization::JSON>::toJson(nlohmann::json &recipient, const FlowSnapshot &structure) {
  std::unordered_map<std::size_t , std::vector<std::size_t>> dependencies_map;
  for(const auto& element : structure) {
    for(auto d : element.dependants) {
      dependencies_map[d].emplace_back(element.id);
    }
  }
  recipient = nlohmann::json::array();
  std::string label;
  for(const auto& element : structure) {
    auto& added = recipient.emplace_back();
    added["has_value"] = element.has_value;
    added["id"] = element.id;
    added["generation"] = element.generation;
    added["label"] = make_label_name(label, element);
    added["dependants_ids"] = element.dependants;

    auto it = dependencies_map.find(element.id);
    if(it == dependencies_map.end()) {
      static const std::vector<std::size_t> place_hoder;
      added["dependencies_ids"] = place_hoder;
    }
    else {
      added["dependencies_ids"] = dependencies_map.at(element.id);
    }
  }
}

void Converter<Serialization::JSON>::toJsonFile(const std::filesystem::path &filename, const FlowSnapshot &structure) {
  auto stream = make_stream(filename);
  nlohmann::json res;
  toJson(res, structure);
  *stream << res.dump(1);
}
#endif

} // namespace flw
