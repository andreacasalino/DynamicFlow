/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <Error.h>

namespace flw {
    Error::Error(const std::string& what)
        : std::runtime_error(what) {
    }
}
