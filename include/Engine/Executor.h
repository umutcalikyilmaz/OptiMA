#pragma once
#include <mutex>
#include <thread>
#include <Engine/IDriver.h>
#include <Engine/IExecutor.h>
#include <Engine/IListener.h>
#include <Engine/PluginManager.h>
#include <Engine/ExecutorState.h>

namespace OptiMA
{
    class Executor : public IExecutor
    {
        IDriver* driver;
        IListener* listener;
        PluginManager* pmanager;
        TransactionQueue* txnQueue;
        ExecutorState** states;
        thread* threads;
        map<int, mutex> pluginLocks;
        const vector<int> haltingAgents;
        int threadNum;
        int count = 0;
        atomic_bool lock;
        bool optimized;
        bool keepStats;

        void LockPlugins(const set<int>& plugins);

        void UnlockPlugins(const set<int>& plugins);

        void ExecuteTransaction(unique_ptr<ITransaction> txn, ExecutorState* state);

        void Run(ExecutorState* state);

    public:

        Executor(IDriver* driver, PluginManager* pmanager, int threadNum, const set<int>& nonshareablePlugins,
        const vector<int>& haltingAgents, bool optimized, bool keepStats);

        void InsertTransactionQueue(TransactionQueue* txnQueue);

        void InsertListener(IListener* listener);

        void Start();

        void AssignTransaction(unique_ptr<ITransaction> txn, int index) override;

        void Stop();

        map<int,map<int,map<int,double>>> GetStats();

        ~Executor();
    };
}