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
            bool isBlocked = true;

            auto it = toUpdate.begin();
            while (it != toUpdate.end()) {
                auto res = (*it)->evaluate();
                if ((EvaluationResult::SUCCESS == res) ||
                    (EvaluationResult::BLOCKING_EXCEPTION == res)) {
                    it = toUpdate.erase(it);

                    isBlocked = false;
                }
                else {                   
                    // EvaluationResult::NOT_READY 
                    ++it;
                }
            }

            if (isBlocked) {
                //throw Error("Something went wrong with the Flow update");
                return;
            }
        }
    }

    bool Flow::isBusy() const {
        return busy;
    }

    template<typename Condition>
    void waitInfinite(Condition condition) {
        while (condition()) {
        }
    }

    template<typename Condition>
    void waitFinite(Condition condition, const std::chrono::microseconds& maxWaitTime) {
        std::chrono::microseconds elapsed(0);
        while (true) {
            auto tic = std::chrono::high_resolution_clock::now();
            if (!condition()) {
                return;
            }
            elapsed += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - tic);
            if (elapsed.count() >= maxWaitTime.count()) {
                return;
            }
        }
    }

    void Flow::waitUpdateComplete(const std::chrono::microseconds& maxWaitTime) const {
        if (0 == maxWaitTime.count()) {
            waitInfinite([this]() {
                return this->isBusy();
            });
        }
        else {
            waitFinite([this]() {
                return this->isBusy();
            }, maxWaitTime);
        }
    }
}
