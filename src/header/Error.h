/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_ERROR_H
#define FLOW_ERROR_H

#include <stdexcept>

namespace flw {
    /** @brief A runtime error that can be raised when using any object in flw::
	 */
    class Error : public std::runtime_error {
    public:
        inline Error(const std::string& what) 
            : std::runtime_error(what) { 
        };
    };
}

#endif