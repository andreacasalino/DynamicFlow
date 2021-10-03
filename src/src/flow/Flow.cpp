/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/Flow.h>

namespace flw {
    std::set<EvaluateCapable*> Flow::computeUpdateRequired(const std::set<EvaluateCapable*>& initialNodes) {
        std::set<EvaluateCapable*> open = initialNodes, close;
        while (!open.empty()) {
            auto* front = *open.begin();
            open.erase(open.begin());
            DescendantsAware* impl = dynamic_cast<DescendantsAware*>(front);
            for (auto* d: impl->descendants) {
                open.emplace(d);
            }
            close.emplace(front);
        }
        return close;
    }

    void Flow::updateNodes(std::set<EvaluateCapable*> toUpdate) {
        while (!toUpdate.empty()) {
            auto it = toUpdate.begin();
            while (it != toUpdate.end()) {
                auto res = (*it)->evaluate();
                if ((EvaluationResult::SUCCESS == res) ||
                    (EvaluationResult::BLOCKING_EXCEPTION == res)) {
                    it = toUpdate.erase(it);
                }
                else {                   
                    // EvaluationResult::NOT_READY 
                    ++it;
                }
            }
        }
    }
}
