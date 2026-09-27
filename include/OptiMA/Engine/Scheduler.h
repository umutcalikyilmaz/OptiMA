#pragma once
#include <thread>
#include "OptiMA/Engine/IExecutor.h"
#include "OptiMA/Engine/IScheduler.h"
#include "OptiMA/Engine/SchedulerSettings.h"

namespace OptiMA
{
    class Scheduler : public IScheduler
    {
    public:

        class DriverKey
        {
        private:

            DriverKey() {}
        
            friend class Driver;
        };

        Scheduler(DriverKey, SchedulerSettings* settings, IExecutor* executor,
            const std::set<int>& nonShareablePlugins, int threadNum);

        void insertTransactionQueue(DriverKey, TransactionQueue* txnQueue);

        void startScheduling(DriverKey);

        ~Scheduler();

    private: 

        IExecutor* executor_;
        TransactionQueue* txnQueue_;
        std::unique_ptr<TxnSP::Solver> slv_;
        TxnSP::SolverInput sinp_;
        const std::set<int> nonShareablePlugins_;
        void (Scheduler::*optimizePtr_)();
        std::vector<std::unique_ptr<ITransaction>> txns_;
        std::mutex deleteLock_;
        std::condition_variable cv_;
        std::vector<double> lengths_;
        std::vector<double> total_;
        std::vector<int> order_;
        std::vector<int> norder_;
        std::vector<std::vector<uint8_t>> conflicts_;
        int threadNum_;
        int txnNum_;
        std::atomic_bool running_;
        std::atomic_bool stopped_;
        bool optimized_;

        void findConflicts();

        TxnSP::SolverOutput createPlan();

        TxnSP::SolverOutput optimize();

        void noPermutation();

        void permutation();
    };
}