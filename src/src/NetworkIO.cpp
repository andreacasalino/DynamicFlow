#include <DynamicFlow/NetworkIO.h>

#include <fstream>

namespace flw {
namespace {
void print(const std::string &filename, const std::string &to_print) {
  std::ofstream stream(filename);
  if (!stream.is_open()) {
    throw Error{filename, " is not a valid file name"};
  }
  stream << to_print << std::endl;
}

static const std::string SEPARATOR = "{}";

std::string format(const std::string &subject,
                   const std::vector<std::string> &values) {
  std::ostringstream result;
  auto valuesIt = values.begin();
  std::size_t pos = 0;
  while (pos < subject.size()) {
    auto nextSepBegin = subject.find(SEPARATOR, pos);
    if (nextSepBegin == std::string::npos) {
      result << std::string{subject, pos};
      break;
    }
    result << std::string{subject, pos, nextSepBegin - pos};
    result << *valuesIt;
    ++valuesIt;
    pos = nextSepBegin + SEPARATOR.size();
  }
  return result.str();
}

std::string to_string(ValueStatus status) {
  switch (status) {
  case ValueStatus::UNSET:
    return R"(<font color="red">UNSET</font>)";
  case ValueStatus::VALUE:
    return R"(<font color="darkgreen">VALUE</font>)";
  // case ValueStatus::EXCEPTION:
  default:
    break;
  }
  return R"(<font color="red">EXCEPTION</font>)";
}
} // namespace

std::string to_dot(const FlowSnapshot &structure) {
  std::ostringstream result;

  auto findNode = [&structure](std::size_t id) {
    return std::find_if(
        structure.begin(), structure.end(),
        [&id](const ValueSnapshot &node) { return node.id == id; });
  };

  auto makeNodeName = [](const ValueSnapshot &node) {
    std::ostringstream result;
    if (node.label) {
      result << node.label.value();
    } else {
      result << "Un-named-" << node.id;
    }
    return result.str();
  };

  result << "digraph DynamicFlow {\n";
  result << R"(node [shape=plaintext fontname="Sans serif" fontsize="8"];)"
         << "\n\n";
  for (const auto &node : structure) {
    const auto name = makeNodeName(node);
    result << format(R"({} [ label=<
                              <table border="1" cellborder="0" cellspacing="1">
                                <tr><td align="left"><b>{}</b></td></tr>
                                <tr><td align="left">epoch: {}</td></tr>
                                <tr><td align="left">status: {}</td></tr>
                                <tr><td align="left">value: {}</td></tr>
                              </table>>];)",
                     {std::to_string(node.id), name, std::to_string(node.epoch),
                      to_string(node.status), node.value_serialization});
    if (node.dependencies) {
      for (const auto &dep_id : node.dependencies.value()) {
        result << format("{} -> {};\n",
                         {std::to_string(dep_id), std::to_string(node.id)});
      }
    }
    result << '\n';
  }
  result << "}\n";

  return result.str();
}

void to_dot(const std::string &filename, const FlowSnapshot &structure) {
  print(filename, to_dot(structure));
}

#ifdef DYNAMIC_FLOW_ENABLE_JSON_EXPORT
namespace {
void toJson(nlohmann::json &recipient, const ValueSnapshot &info) {
  recipient["id"] = info.id;
  if (info.label) {
    recipient["label"] = info.label.value();
  }
  recipient["epoch"] = info.epoch;
  switch (info.status) {
  case ValueStatus::UNSET:
    recipient["status"] = "UNSET";
    break;
  case ValueStatus::VALUE:
    recipient["status"] = "VALUE";
    break;
  case ValueStatus::EXCEPTION:
    recipient["status"] = "EXCEPTION";
    break;
  }
  // recipient["value_serialization"] = info.value_serialization;
  if (info.dependencies) {
    recipient["dependencies"] = info.dependencies.value();
  }
}
} // namespace

void to_json(nlohmann::json &recipient, const FlowSnapshot &structure) {
  recipient = nlohmann::json::array();
  for (const auto &info : structure) {
    toJson(recipient.emplace_back(), info);
  }
}

void to_json(const std::string &filename, const FlowSnapshot &structure) {
  nlohmann::json asJson;
  to_json(asJson, structure);
  print(filename, asJson.dump());
}
#endif

} // namespace flw
