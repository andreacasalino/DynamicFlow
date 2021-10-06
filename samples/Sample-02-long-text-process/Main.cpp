#include <TextImport.h>
#include <WordsParser.h>
#include <flow/Flow.h>

#include <iostream>
#include <map>
#include <vector>
using namespace flw;

void make_text_analysis_nodes(Flow &flow, const std::string &text_name) {
  auto location = flow.makeSource<std::string>(text_name);

  auto content = flow.makeNode(
      location.getName() + std::string("-content"),
      std::function<std::list<std::string>(const std::string &)>(
          [](const std::string &file_path) { return importText(file_path); }),
      location);

  auto linesCounter = flow.makeNode(
      location.getName() + std::string("-lines-counter"),
      std::function<std::size_t(const std::list<std::string> &)>(
          [](const std::list<std::string> &content) { return content.size(); }),
      content);

  auto spacesCounter =
      flow.makeNode(location.getName() + std::string("-spaces-counter"),
                    std::function<std::size_t(const std::list<std::string> &)>(
                        [](const std::list<std::string> &content) {
                          std::size_t result = 0;
                          for (const auto &line : content) {
                            result += countSpaces(line);
                          }
                          return result;
                        }),
                    content);

  auto wordsByLine =
      flow.makeNode(location.getName() + std::string("-words-by-lines"),
                    std::function<std::vector<std::list<std::string>>(
                        const std::list<std::string> &)>(
                        [](const std::list<std::string> &content) {
                          std::vector<std::list<std::string>> result;
                          result.reserve(content.size());
                          for (const auto &line : content) {
                            result.push_back(parseWords(line));
                          }
                          return result;
                        }),
                    content);

  auto wordsFrequencies = flow.makeNode(
      location.getName() + std::string("-words-frequencies"),
      std::function<std::map<std::string, std::size_t>(
          const std::vector<std::list<std::string>> &)>(
          [](const std::vector<std::list<std::string>> &wordsByLine) {
            std::map<std::string, std::size_t> result;
            for (const auto &line : wordsByLine) {
              for (const auto &word : line) {
                auto it = result.find(word);
                if (it == result.end()) {
                  result.emplace(word, 1);
                } else {
                  ++it->second;
                }
              }
            }
            return result;
          }),
      wordsByLine);

  auto wordsCounter = flow.makeNode(
      location.getName() + std::string("-words-counter"),
      std::function<std::size_t(const std::map<std::string, std::size_t> &)>(
          [](const std::map<std::string, std::size_t> &wordsFrequencies) {
            std::size_t result = 0;
            for (auto it = wordsFrequencies.begin();
                 it != wordsFrequencies.end(); ++it) {
              result += it->second;
            }
            return result;
          }),
      wordsFrequencies);
}

int main() {
  // build the flow
  flw::Flow flow;

  // build initial nodes for reading and parsing the first text
  const std::string first_text = "Dummy-text";
  make_text_analysis_nodes(flow, first_text);

  // set the inputs and update the flow
  flow.updateFlow(first_text, std::make_unique<std::string>(
                                  std::string(SAMPLE_PATH) + first_text));
  flow.waitUpdateComplete();

  return EXIT_SUCCESS;
}
