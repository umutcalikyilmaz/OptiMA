#include "OptiMA/Engine/ExecutorState.h"

namespace OptiMA
{
    ExecutorState::ExecutorState() : txnQueue(new TransactionQueue()), started(false), running(false), optimized(true) { }

    ExecutorState::ExecutorState(TransactionQueue* txnQueue) : txnQueue(txnQueue), started(false), running(false),
    optimized(false) { }

    ExecutorState::~ExecutorState()
    {
        if(optimized)
        {
            delete txnQueue;
        }
    }
}