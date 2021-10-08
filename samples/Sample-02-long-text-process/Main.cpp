#include <TextIO.h>
#include <WordsParser.h>
#include <flow/Printer.h>

#include <fstream>
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

void show_text_analysis_result(Flow &flow, const std::string &text_name) {
  std::cout << text_name << "  analysis" << std::endl;

  auto linesCounter =
      flow.template findNode<std::size_t, std::list<std::string>>(
          text_name + std::string("-lines-counter"));
  std::cout << "lines: " << copyValue(linesCounter) << std::endl;

  auto spacesCounter =
      flow.template findNode<std::size_t, std::list<std::string>>(
          text_name + std::string("-spaces-counter"));
  std::cout << "spaces: " << copyValue(spacesCounter) << std::endl;

  auto wordsCounter =
      flow.template findNode<std::size_t, std::map<std::string, std::size_t>>(
          text_name + std::string("-words-counter"));
  std::cout << "words: " << copyValue(wordsCounter) << std::endl;

  auto content = flow.template findNode<std::list<std::string>, std::string>(
      text_name + std::string("-content"));
  std::cout << "content: " << std::endl;
  content.useValue([](const std::list<std::string> &lines) {
    for (const auto &line : lines) {
      std::cout << line << std::endl;
    }
  });

  auto wordsFrequencies =
      flow.template findNode<std::map<std::string, std::size_t>,
                             std::vector<std::list<std::string>>>(
          text_name + std::string("-words-frequencies"));
  std::cout << "words frequencies: " << std::endl;
  wordsFrequencies.useValue([](const std::map<std::string, std::size_t> &freq) {
    for (auto it = freq.begin(); it != freq.end(); ++it) {
      std::cout << it->first << "  :  " << it->second << std::endl;
    }
  });
}

void sep() {
  std::cout << "----------------------------------" << std::endl << std::endl;
}

static const std::string FIRST_TEXT = "LoremIpsum";
static const std::string SECOND_TEXT = "Novel";

int main() {

  // build the flow
  flw::PrintableFlow flow;

  // try to read and analyze an inexistent file
  make_text_analysis_nodes(flow, FIRST_TEXT);
  // log the flow entities
  flw::log("Log", flow);
  // // set the inputs and update the flow
  // flow.updateSourcesAndFlow(FIRST_TEXT,
  //                           std::make_unique<std::string>(FIRST_TEXT));
  // show_text_analysis_result(flow, FIRST_TEXT);
  // sep();

  //  make_text_analysis_nodes(flow, first_text);
  //  make_text_analysis_nodes(flow, second_text);
  //
  ////  auto first_content = flow.findNode<std::list<std::string>, std::string>(
  ////      first_text + std::string("-content"));
  ////  auto second_content = flow.findNode<std::list<std::string>,
  /// std::string>( /      second_text + std::string("-content")); /  auto
  /// combined_node = flow.makeNode( /      "combined-text", /
  /// std::function<std::list<std::string>(const std::list<std::string> &, /
  /// const std::list<std::string> &)>( /          [](const
  /// std::list<std::string> &contentA, /             const
  /// std::list<std::string> &contentB) { /            std::list<std::string>
  /// mixed; /            auto itA = contentA.begin(); /            auto itB =
  /// contentB.begin(); /            while ((itA != contentA.end()) && (itB !=
  /// contentB.end())) { /              if (itA != contentA.end()) { /
  /// mixed.push_back(*itA); /                ++itA; /              } / if (itB
  ///!= contentB.end()) { /                mixed.push_back(*itB); / ++itB; / }
  ////            }
  ////            return mixed;
  ////          }),
  ////      first_content, second_content);
  //
  //  // set the inputs and update the flow
  //  flow.updateSourcesAndFlow(
  //      first_text,
  //      std::make_unique<std::string>(std::string(SAMPLE_PATH) + first_text));
  //  show_text_analysis_result(flow, first_text);
  //  std::cout <<
  //  "---------------------------------------------------------------"
  //            << std::endl;
  //  show_text_analysis_result(flow, second_text);
  //
  //
  ////  combined_node.useValue([](const std::list<std::string> &lines) {
  ////    std::ofstream stream("Combined");
  ////    for (const auto &line : lines) {
  ////      stream << line << std::endl;
  ////    }
  ////  });

  return EXIT_SUCCESS;
}
