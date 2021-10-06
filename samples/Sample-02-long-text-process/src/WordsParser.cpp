#include <WordsParser.h>

std::size_t countSpaces(const std::string &line) {
  std::size_t result = 0;
  for (std::size_t k = 0; k < line.size(); ++k) {
    if (' ' == line[k]) {
      ++result;
    }
  }
  return result;
}

#include <Error.h>
std::list<std::string> parseWords(const std::string &line) {
  throw flw::Error("still to implement");
}
