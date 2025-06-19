#pragma once
#include <thread>
#include "OptiMA/Engine/IExecutor.h"
#include "OptiMA/Engine/IScheduler.h"
#include "OptiMA/Engine/SchedulerSettings.h"

namespace OptiMA
{
    class Scheduler : public IScheduler
    {
    private: 

        IExecutor* executor_;
        TransactionQueue* txnQueue_;
        TxnSP::Solver* slv_;
        TxnSP::SolverInput sinp_;
        const set<int> nonShareablePlugins_;
        void (Scheduler::*optimizePtr_)();
        vector<unique_ptr<ITransaction>> txns_;
        mutex deleteLock_;
        condition_variable cv_;
        double* lengths_;
        double* total_;
        int* order_;
        int* norder_;
        bool** conflicts_;
        int threadNum_;
        int txnNum_;
        atomic_bool running_;
        atomic_bool stopped_;
        bool perm_;
        bool optimized_;

        void findConflicts();

        void createPlan(TxnSP::SolverOutput*& out);

        void optimize(TxnSP::SolverOutput*& out);

        void noPermutation();

        void permutation();

        Scheduler(SchedulerSettings* settings, IExecutor* executor, const set<int>& nonShareablePlugins, int threadNum);

        void insertTransactionQueue(TransactionQueue* txnQueue);

        void startScheduling();
        
        ~Scheduler();

        friend class Driver;
    };
}