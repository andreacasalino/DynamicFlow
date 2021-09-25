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
    class FlowName
        : public std::shared_ptr<const std::string> {
    public:
        FlowName(const std::string& nameRaw);
    };

    inline bool operator<(const FlowName& a, const FlowName& b) {
        return *a.get() < *b.get();
    }

    class FlowEntity {
    public:
        virtual ~FlowEntity() = default;

        inline FlowName getName() const { return name; }

    protected:
        FlowEntity(const std::string& nameRaw);

    private:
        FlowName name;
    };
    using FlowEntityPtr = std::shared_ptr<FlowEntity>;
}

#endif
