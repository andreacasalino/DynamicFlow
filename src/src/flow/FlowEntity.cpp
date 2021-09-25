/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/FlowEntity.h>

namespace flw {
    FlowEntity::FlowEntity(const std::string& nameRaw) {
        name = std::make_shared<const std::string>(nameRaw);
    };
}
