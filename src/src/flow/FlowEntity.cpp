/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/FlowEntity.h>
#include <sstream>

namespace flw {
    static long long unamed_counter = 0;

    FlowEntity::FlowEntity(const std::string& nameRaw) {
        if (nameRaw.empty()) {
            std::stringstream stream;
            stream << "Unamed-" << unamed_counter;
            ++unamed_counter;
            name = std::make_shared<const std::string>(stream.str());
            return;
        }
        name = std::make_shared<const std::string>(nameRaw);
    };
}
