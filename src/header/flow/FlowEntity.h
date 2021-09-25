/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_FLOW_ENTITY_H
#define FLOW_FLOW_ENTITY_H

#include <string>
#include <memory>

namespace flw {
    using FlowName = std::shared_ptr<const std::string>;

    inline bool operator<(const FlowName& a, const FlowName& b) {
        return *a.get() < *b.get();
    }

    class FlowEntity {
    public:
        virtual ~FlowEntity() = default;

    protected:
        FlowEntity(const std::string& nameRaw);

        inline FlowName getName() const { return name; }

    private:
        FlowName name;
    };
    using FlowEntityPtr = std::shared_ptr<FlowEntity>;
}

#endif
