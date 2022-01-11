/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <components/DescendantsAware.hpp>
#include <flow/Printer.h>
#include <fstream>
#include <set>

namespace flw {

namespace {
void printStatus(std::ostream &stream, const ValueOrExceptionAware &entity) {
  if (entity.isValue()) {
    stream << "HAS_VALUE";
    return;
  }
  if (entity.isException()) {
    stream << "EXCEPTION: ";
    try {
      std::rethrow_exception(entity.getException());
    } catch (const Error &e) {
      stream << " Error: " << e.what();
    } catch (const std::exception &e) {
      stream << e.what();
    }
    return;
  }
  stream << "NULL";
}

constexpr std::size_t DEFAULT_SEP_SIZE = 3;

template <std::size_t Size = DEFAULT_SEP_SIZE> struct Separator {
  Separator() = default;
};
template <std::size_t Size>
std::ostream &operator<<(std::ostream &stream,
                         [[maybe_unused]] const Separator<Size> &sep) {
  for (std::size_t k = 0; k < Size; ++k) {
    stream << ' ';
  }
  return stream;
};
const Separator DEFAULT_SEPARATOR;

template <typename EntityContainer>
void printNames(std::ostream &stream, const EntityContainer &collection) {
  stream << '[';
  if (!collection.empty()) {
    auto it = collection.begin();
    stream << *(*it)->getName();
    ++it;
    while (it != collection.end()) {
      stream << DEFAULT_SEPARATOR << *(*it)->getName();
      ++it;
    }
  }
  stream << ']';
}

struct EntityInfo {
  const FlowEntity *entity;
  std::set<const FlowEntity *> descendants;
};

std::list<const FlowEntity *> getParents(const FlowEntity *subject,
                                         const std::vector<EntityInfo> &data) {
  std::list<const FlowEntity *> parents;
  for (const auto &d : data) {
    if (d.descendants.find(subject) != d.descendants.end()) {
      parents.push_back(d.entity);
    }
  }
  return parents;
}
} // namespace

void PrintBasic::print(std::ostream &stream) const {
  std::vector<EntityInfo> data;
  for (const auto &[name, entity] : allTogether) {
    data.emplace_back(EntityInfo{entity.get(), std::set<const FlowEntity *>{}});
    const auto *asDescAware =
        dynamic_cast<const DescendantsAware *>(entity.get());
    if (nullptr == asDescAware) {
      throw Error("Bad casting when printing");
    }
    for (auto d : asDescAware->descendants) {
      const auto *asEntity = dynamic_cast<const FlowEntity *>(d);
      if (nullptr == asEntity) {
        throw Error("Bad casting when printing");
      }
      data.back().descendants.emplace(asEntity);
    }
  }
  for (const auto &d : data) {
    stream << "name:" << *d.entity->getName() << DEFAULT_SEPARATOR;

    const auto *asValueAware =
        dynamic_cast<const ValueOrExceptionAware *>(d.entity);
    if (nullptr == asValueAware) {
      throw Error("Bad casting when printing");
    }
    stream << " status:";
    printStatus(stream, *asValueAware);
    stream << DEFAULT_SEPARATOR;

    stream << " parents:";
    printNames(stream, getParents(d.entity, data));
    stream << DEFAULT_SEPARATOR;

    stream << " descendants:";
    printNames(stream, d.descendants);

    stream << std::endl;
  }
}

void log(const std::string &fileName, const flw::PrintCapable &subject) {
  std::ofstream stream(fileName);
  if (!stream.is_open()) {
    throw Error("Invalid log file location");
  }
  subject.print(stream);
}

} // namespace flw

std::ostream &operator<<(std::ostream &stream,
                         const flw::PrintCapable &subject) {
  subject.print(stream);
  return stream;
}
