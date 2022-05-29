#include <DynamicFlow/flow/Printer.h>
#include <FlowLogger.h>
#include <TextIO.h>
#include <WordsParser.h>

#include <iostream>
#include <map>
#include <vector>
using namespace flw;

void make_text_analysis_nodes(Flow &flow, const std::string &text_name);
void make_text_analysis_exporter_node(Flow &flow, const std::string &text_name,
                                      const std::string &file_name);

static const std::string FIRST_TEXT = "LoremIpsum";
static const std::string SECOND_TEXT = "Novel";

int main() {
  // FlowLogger::log(flow);

  // build the flow
  flw::PrintableFlow flow;

  // import the first text
  make_text_analysis_nodes(flow, FIRST_TEXT);
  // set the inputs and update the flow
  flow.updateSourcesAndFlow(FIRST_TEXT,
                            std::make_unique<std::string>(FIRST_TEXT));
  // log the flow structure
  FlowLogger::log(flow);
  // create a node to export the results
  const std::string first_text_analysis_file = "First-Text-Analysis";
  make_text_analysis_exporter_node(flow, FIRST_TEXT, first_text_analysis_file);
  FlowLogger::log(flow);
  // udpate the flow to generate the value in the newly created node
  flow.updateFlow();
  FlowLogger::log(flow);
  std::cout << "done" << std::endl;

  // do the same for the second text
  make_text_analysis_nodes(flow, SECOND_TEXT);
  flow.updateSourcesAndFlow(SECOND_TEXT,
                            std::make_unique<std::string>(SECOND_TEXT));
  const std::string second_text_analysis_file = "Second-Text-Analysis";
  make_text_analysis_exporter_node(flow, SECOND_TEXT,
                                   second_text_analysis_file);
  FlowLogger::log(flow);
  flow.updateFlow();
  FlowLogger::log(flow);
  std::cout << "done" << std::endl;

  // create a text that combines the other two
  auto first_content = flow.findNode<std::list<std::string>>(
      FIRST_TEXT + std::string("-content"));
  auto second_content = flow.findNode<std::list<std::string>>(
      SECOND_TEXT + std::string("-content"));
  auto combined_node = flow.makeNode(
      "combined-text",
      std::function<std::list<std::string>(const std::list<std::string> &,
                                           const std::list<std::string> &)>(
          [](const std::list<std::string> &contentA,
             const std::list<std::string> &contentB) {
            std::list<std::string> mixed;
            auto itA = contentA.begin();
            auto itB = contentB.begin();
            while ((itA != contentA.end()) && (itB != contentB.end())) {
              if (itA != contentA.end()) {
                mixed.push_back(*itA);
                ++itA;
              }
              if (itB != contentB.end()) {
                mixed.push_back(*itB);
                ++itB;
              }
            }
            return mixed;
          }),
      first_content, second_content);
  // udpate the flow to generate the value in the newly created node
  FlowLogger::log(flow);
  flow.updateFlow();
  FlowLogger::log(flow);

  // since the flow is updated when reaching this point, simply use the lastest
  // computed value inside the node
  combined_node.useValue([](const std::list<std::string> &lines) {
    exportText(lines, "CombinedText");
  });
  std::cout << "Combined text created" << std::endl;

  return EXIT_SUCCESS;
}

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

void make_text_analysis_exporter_node(Flow &flow, const std::string &text_name,
                                      const std::string &file_name) {
  auto linesCounter = flow.template findNode<std::size_t>(
      text_name + std::string("-lines-counter"));

  auto spacesCounter = flow.template findNode<std::size_t>(
      text_name + std::string("-spaces-counter"));

  auto wordsCounter = flow.template findNode<std::size_t>(
      text_name + std::string("-words-counter"));

  auto wordsFrequencies =
      flow.template findNode<std::map<std::string, std::size_t>>(
          text_name + std::string("-words-frequencies"));

  auto export_was_done = flow.makeNode(
      text_name + std::string("-export-results"),
      std::function<bool(const std::size_t &, const std::size_t &,
                         const std::size_t &,
                         const std::map<std::string, std::size_t> &)>(
          [&file_name,
           &text_name](const std::size_t &lines, const std::size_t &spaces,
                       const std::size_t &words,
                       const std::map<std::string, std::size_t> &frequencies) {
            auto stream = make_out_stream(file_name);
            if (stream.isException()) {
              std::rethrow_exception(stream.getException());
            }
            std::cout << file_name << std::endl;

            *stream.get() << text_name << "  analysis" << std::endl;

            *stream.get() << "<<<------------------>>>" << std::endl;

            *stream.get() << "lines: " << lines << std::endl;

            *stream.get() << "spaces: " << spaces << std::endl;

            *stream.get() << "words: " << words << std::endl;

            *stream.get() << "<<<------------------>>>" << std::endl;

            *stream.get() << "words frequencies: " << std::endl;
            for (auto it = frequencies.begin(); it != frequencies.end(); ++it) {
              *stream.get() << it->first << "  :  " << it->second << std::endl;
            }

            return true;
          }),
      linesCounter, spacesCounter, wordsCounter, wordsFrequencies);
}
