/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_DESCENDANTS_AWARE_H
#define FLOW_DESCENDANTS_AWARE_H

#include <components/EvaluateCapable.h>
#include <list>
#include <mutex>

namespace flw {

    class DescendantsAware {
    public:
        mutable std::mutex descendantsMtx;
        mutable std::list<EvaluateCapable*> descendants;

    protected:
        DescendantsAware() = default;
    };

}

#endif
