#include "RunScript.h"

namespace flw::sample {
void appendArgs_(std::ostringstream &stream) { return; }

void runShowGraph(const std::string &dot_file_to_render) {
  runPython(SHOW_GRAPH_PYTHON, "--file", dot_file_to_render);
}
} // namespace flw::sample
