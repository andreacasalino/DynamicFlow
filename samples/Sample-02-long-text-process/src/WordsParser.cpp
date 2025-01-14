#include <WordsParser.h>

#include <algorithm>

namespace flw::sample {
std::size_t countSpaces(const std::string &line) {
  return std::count(line.begin(), line.end(), ' ');
}

std::list<std::string> parseWords(const std::string &line) {
  std::list<std::string> result;
  std::string formingWord;
  for (auto c : line) {
    if (' ' == c) {
      if (!formingWord.empty()) {
        result.push_back(formingWord);
      }
      formingWord.clear();
    } else {
      formingWord += c;
    }
  }
  if(!formingWord.empty()) {
    result.push_back(formingWord);
  }
  return result;
}
} // namespace flw::sample
