#include <iostream>
#include <map>
#include <vector>

#include <DynamicFlow/Network.hxx>
#include <DynamicFlow/NetworkSerialization.h>

#include <RunScript.h>
#include <TextIO.h>
#include <WordsParser.h>

void build_network(flw::Flow &flow);

static const std::string FIRST_TEXT = "LoremIpsum";
static const std::string SECOND_TEXT = "Novel";

int main() {
  // build the flow
  flw::Flow flow;
  flow.setOnNewNodePolicy(flw::HandlerMaker::OnNewNodePolicy::DEFERRED_UPDATE);
  build_network(flow);

  // take a snapshot of the network and export it as a .dot file
  flw::Converter<flw::Serialization::DOT>::toFile(
      flw::sample::LogDir::get() / "Flow-Sample-02.dot", flow.snapshot());
  // use python graphviz to render exported .dot file
  flw::sample::RunScript::runDefaultScript("Flow-Sample-02.dot");

  auto fileNameSource = flow.findSource<std::string>("fileName");

  // process the first text
  fileNameSource.update(FIRST_TEXT);
  flow.update();

  // now a non existing file ... a call back on the FileNotFound exception was
  // registered and should be printed
  fileNameSource.update("InexistentFile");
  flow.update();

  // process the second text
  fileNameSource.update(SECOND_TEXT);
  flow.update();

  return EXIT_SUCCESS;
}

void append_analysis_nodes(flw::Flow &flow) {
  auto fileName = flow.makeSource<std::string>("", "fileName");

  auto content = flow.makeNodeWithErrorsCB<std::list<std::string>, std::string>(
      [](const std::string &file_name) {
        return flw::sample::importText(file_name);
      },
      fileName,
      flw::ValueCallBacks<std::list<std::string>, flw::sample::FileNotFound>{}
          .addOnError<flw::sample::FileNotFound>(
              [](const flw::sample::FileNotFound &e) {
                std::cout << e.what() << std::endl;
              })
          .extract(),
      "content");

  auto linesCounter = flow.makeNode<std::size_t, std::list<std::string>>(
      [](const std::list<std::string> &content) { return content.size(); },
      content, "lines-counter");

  auto spacesCounter = flow.makeNode<std::size_t, std::list<std::string>>(
      [](const std::list<std::string> &content) {
        std::size_t result = 0;
        for (const auto &line : content) {
          result += flw::sample::countSpaces(line);
        }
        return result;
      },
      content, "spaces-counter");

  auto wordsByLine = flow.makeNode<std::vector<std::list<std::string>>,
                                   std::list<std::string>>(
      [](const std::list<std::string> &content) {
        std::vector<std::list<std::string>> result;
        result.reserve(content.size());
        for (const auto &line : content) {
          result.push_back(flw::sample::parseWords(line));
        }
        return result;
      },
      content, "words-by-lines");

  auto wordsFrequencies = flow.makeNode<std::map<std::string, std::size_t>,
                                        std::vector<std::list<std::string>>>(
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
      },
      wordsByLine, "words-frequencies");

  auto wordsCounter =
      flow.makeNode<std::size_t, std::map<std::string, std::size_t>>(
          [](const std::map<std::string, std::size_t> &wordsFrequencies) {
            std::size_t result = 0;
            for (auto it = wordsFrequencies.begin();
                 it != wordsFrequencies.end(); ++it) {
              result += it->second;
            }
            return result;
          },
          wordsFrequencies, "words-counter");
}

void append_exporter_node(flw::Flow &flow) {
  auto fileName = flow.findSource<std::string>("fileName");
  auto linesCounter = flow.findNode<std::size_t>("lines-counter");
  auto spacesCounter = flow.findNode<std::size_t>("spaces-counter");
  auto wordsCounter = flow.findNode<std::size_t>("words-counter");
  auto wordsFrequencies =
      flow.findNode<std::map<std::string, std::size_t>>("words-frequencies");

  flow.makeNode<std::filesystem::path, std::string, std::size_t, std::size_t,
                std::size_t, std::map<std::string, std::size_t>>(
      [](const std::string &fileName, const std::size_t &lines,
         const std::size_t &spaces, const std::size_t &words,
         const std::map<std::string, std::size_t> &frequencies) {
        std::string outputFile = fileName + "_results.log";

        auto stream = flw::sample::make_out_stream(outputFile);

        *stream.get() << fileName << "  analysis" << std::endl;

        *stream.get() << "<<<------------------>>>" << std::endl;

        *stream.get() << "lines: " << lines << std::endl;

        *stream.get() << "spaces: " << spaces << std::endl;

        *stream.get() << "words: " << words << std::endl;

        *stream.get() << "<<<------------------>>>" << std::endl;

        *stream.get() << "words frequencies: " << std::endl;
        for (auto it = frequencies.begin(); it != frequencies.end(); ++it) {
          *stream.get() << it->first << "  :  " << it->second << std::endl;
        }

        return outputFile;
      },
      fileName, linesCounter, spacesCounter, wordsCounter, wordsFrequencies, "",
      [](const std::filesystem::path &outputFile) {
        std::cout << " done, results written in "
                  << std::filesystem::current_path() / outputFile << std::endl;
      });
}

void build_network(flw::Flow &flow) {
  append_analysis_nodes(flow);
  append_exporter_node(flow);
}
