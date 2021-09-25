/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/FlowEntity.h>
#include <sstream>

namespace flw {
    FlowName::FlowName(const std::string& nameRaw)
        : std::shared_ptr<const std::string>(std::make_shared<const std::string>(nameRaw)) {
    }

    static long long unamed_counter = 0;

    std::string make_name(const std::string& nameRaw) {
        if (nameRaw.empty()) {
            std::stringstream stream;
            stream << "Unamed-" << unamed_counter;
            ++unamed_counter;
            return stream.str();
        }
        return nameRaw;
    }

    FlowEntity::FlowEntity(const std::string& nameRaw)
        : name(make_name(nameRaw)) {
    };
}
