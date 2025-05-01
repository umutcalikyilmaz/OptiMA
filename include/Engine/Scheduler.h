#pragma once
#include <thread>
#include <Engine/IExecutor.h>
#include <Engine/IScheduler.h>
#include <Engine/SchedulerSettings.h>

namespace OptiMA
{
    class Scheduler : public IScheduler
    {
        IExecutor* executor;
        TransactionQueue* txnQueue;
        TransactionScheduling::Solver* slv;
        TransactionScheduling::SolverInput sinp;
        const set<int> nonShareablePlugins;
        void (Scheduler::*optimizePtr)();
        vector<unique_ptr<ITransaction>> txns;
        mutex deleteLock;
        condition_variable cv;
        double* lengths;
        double* total;
        int* order;
        int* norder;
        bool** conflicts;
        int threadNum;
        int txnNum;
        atomic_bool running;
        atomic_bool stopped;
        bool perm;
        bool optimized;

        void FindConflicts();

        void CreatePlan(TransactionScheduling::SolverOutput*& out);

        void Optimize(TransactionScheduling::SolverOutput*& out);

        void NoPermutation();

        void Permutation();

        Scheduler(SchedulerSettings* settings, IExecutor* executor, const set<int>& nonShareablePlugins, int threadNum);

        void InsertTransactionQueue(TransactionQueue* txnQueue);

        void StartScheduling();
        
        ~Scheduler();

        friend class Driver;
    };
}