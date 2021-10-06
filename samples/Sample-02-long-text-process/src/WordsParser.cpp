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

std::list<std::string> parseWords(const std::string &line) {
  std::list<std::string> result;
  std::string formingWord;
  for (std::size_t k = 0; k < line.size(); ++k) {
    if (' ' == line[k]) {
      if (!formingWord.empty()) {
        result.push_back(formingWord);
      }
      formingWord.clear();
    } else {
      formingWord.push_back(line[k]);
    }
  }
  return result;
}
