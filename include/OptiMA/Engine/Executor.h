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

        Executor(IDriver* driver, TransactionFactory* tfactory, PluginManager* pmanager, int threadNum, const set<int>& nonshareablePlugins,
        bool optimized, bool trigger, bool keepStats);

        void insertTransactionQueue(TransactionQueue* txnQueue);

        void insertListener(IListener* listener);

        void start();

        void assignTransaction(unique_ptr<ITransaction> txn, int index) override;

        void stop();

        map<int,map<int,double>> getStats();

        ~Executor();

    private:

        IDriver* driver_;
        IListener* listener_;
        PluginManager* pmanager_;
        TransactionFactory* tfactory_;
        TransactionQueue* txnQueue_;
        ExecutorState** states_;
        thread* threads_;
        map<int, unique_ptr<mutex>> pluginLocks_;
        const vector<int> haltingAgents_;
        int threadNum_;
        int count_ = 0;
        atomic_bool lock_;
        bool optimized_;
        bool keepStats_;
        bool trigger_;

        void lockPlugins(const set<int>& plugins);

        void unlockPlugins(const set<int>& plugins);

        void executeTransaction(unique_ptr<ITransaction> txn, ExecutorState* state);

        void run(ExecutorState* state);
    };
}