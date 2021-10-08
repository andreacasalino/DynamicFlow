/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <ostream>
#include <flow/EntityAware.hpp>
#include <flow/Flow.h>

namespace flw {
    class PrintCapable
            : virtual public EntityAware {
    public:
        virtual void print(std::ostream& stream) const = 0;
    };

    class PrintBasic
            : public PrintCapable {
    public:
        void print(std::ostream& stream) const override;
    };

    template<typename  FlowT = Flow, typename  PrintableT = PrintBasic>
    class PrintableFlow
            : public FlowT
            , public PrintableT {
    public:
        PrintableFlow() = default;
    };
}

std::ostream& operator<<(std::ostream& stream, flw::PrintCapable& subject);
