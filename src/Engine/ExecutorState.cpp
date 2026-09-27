#include "OptiMA/Engine/ExecutorState.h"

namespace OptiMA
{
    ExecutorState::ExecutorState(TransactionQueue* txnQueue)
        : txnQueue(txnQueue),
          started(false),
          running(false) { }
}