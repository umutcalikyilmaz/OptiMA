#pragma once
#include <mutex>
#include <thread>
#include "OptiMA/Engine/IDriver.h"
#include "OptiMA/Engine/IExecutor.h"
#include "OptiMA/Engine/PluginManager.h"
#include "OptiMA/Engine/TransactionFactory.h"

namespace OptiMA
{
    class Executor : public IExecutor
    {
    public:
        
        class DriverKey
        {
        private:
            
            DriverKey() {}

            friend class Driver;
        };


        Executor(DriverKey, IDriver* driver, TransactionFactory* tfactory, PluginManager* pmanager,
            int threadNum, const std::set<int>& nonshareablePlugins, bool optimized, bool keepStats);

        void insertTransactionQueue(DriverKey, TransactionQueue* txnQueue);

        void insertListener(DriverKey, IListener* listener);

        void start(DriverKey);

        void assignTransaction(SchedulerKey, std::unique_ptr<ITransaction> txn, int index) override;

        void stop(DriverKey);

        std::map<int,std::map<int,double>> getStats(DriverKey);

    private:

        IDriver* driver_;
        IListener* listener_;
        PluginManager* pmanager_;
        TransactionFactory* tfactory_;
        TransactionQueue* txnQueue_;
        std::vector<std::unique_ptr<TransactionQueue>> txnQueues_;
        std::vector<std::unique_ptr<ExecutorState>> states_;
        std::vector<std::thread> threads_;
        std::map<int, std::unique_ptr<std::mutex>> pluginLocks_;
        const std::vector<int> haltingAgents_;
        int threadNum_;
        int count_ = 0;
        std::atomic_bool lock_;
        bool optimized_;
        bool keepStats_;

        void lockPlugins(const std::set<int>& plugins);

        void unlockPlugins(const std::set<int>& plugins);

        void executeTransaction(std::unique_ptr<ITransaction> txn, ExecutorState* state);

        void run(ExecutorState* state);
    };
}