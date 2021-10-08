/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/Printer.h>
#include <set>
#include <map>
#include <components/DescendantsAware.hpp>

namespace flw {

    namespace {
        void printStatus(std::ostream& stream, const ValueOrExceptionAware& entity) {
            if(entity.isValue()) {
                stream << "HAS_VALUE";
                return;
            }
            if(entity.isException()) {
                stream << "EXCEPTION: ";
                try {
                    std::rethrow_exception(entity.getException());
                }
                catch(const std::exception &e) {
                    stream << e.what();
                }
                return;
            }
            stream << "NULL";
        }
    }

    void PrintBasic::print(std::ostream& stream) const {
        std::map<FlowEntity*, std::set<FlowEntity*>> data;


    }

}

std::ostream& operator<<(std::ostream& stream, flw::PrintCapable& subject) {
    subject.print(stream);
    return stream;
}
