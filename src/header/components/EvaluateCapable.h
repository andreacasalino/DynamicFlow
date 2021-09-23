/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_EVALUATE_CAPABLE_H
#define FLOW_EVALUATE_CAPABLE_H

namespace flw {

    enum EvaluationResult {
        SUCCESS,
        NOT_READY,
        EXCEPTION
    };

    class EvaluateCapable {
    public:
        virtual EvaluationResult evaluate() = 0;
    };

}

#endif
