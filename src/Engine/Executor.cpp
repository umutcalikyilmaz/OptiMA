#include "OptiMA/Engine/Executor.h"

namespace OptiMA
{
    void Executor::lockPlugins(const std::set<int>& plugins)
    {
        for(auto p : plugins)
        {
            pluginLocks_.at(p)->lock();
        }
    }

    void Executor::unlockPlugins(const std::set<int>& plugins)
    {
        for(auto p : plugins)
        {
            pluginLocks_.at(p)->unlock();
        }
    }

    void Executor::executeTransaction(std::unique_ptr<ITransaction> txn, ExecutorState* state)
    {
        if(txn == nullptr)
        {
            return;
        }

        const std::set<int> nonShareablePlugins = txn->getNonShareablePlugins();        

        lockPlugins(nonShareablePlugins);

        double beg = std::chrono::steady_clock::now().time_since_epoch().count();
        std::shared_ptr<TransactionResult> res = txn->execute(typename ITransaction::ExecutorKey {});

        if(keepStats_)
        {            
            double end = std::chrono::steady_clock::now().time_since_epoch().count();            
            state->totalTimes[txn->getType()][txn->getSubType()] += end - beg;
            state->counts[txn->getType()][txn->getSubType()]++;
        }

        unlockPlugins(nonShareablePlugins);

        tfactory_->postProcess(typename TransactionFactory::ExecutorKey {}, move(txn), res);        
    }

    void Executor::run(ExecutorState* state)
    {
        while(state->started)
        {
            std::unique_ptr<ITransaction> txn = state->txnQueue->pull();
            executeTransaction(move(txn), state);            
        }
    }

    Executor::Executor(DriverKey, IDriver* driver, TransactionFactory* tfactory, PluginManager* pmanager,
        int threadNum, const std::set<int>& nonshareablePlugins, bool optimized, bool keepStats) 
        : driver_(driver),
          tfactory_(tfactory),
          pmanager_(pmanager),
          lock_(false),
          threadNum_(threadNum),
          optimized_(optimized),
          keepStats_(keepStats)
    {
        states_.reserve(threadNum);
        threads_.reserve(threadNum);

        if(optimized)
        {
            txnQueues_.reserve(threadNum);
        }

        std::set<int> nonShareable = pmanager_->getNonShareable();

        for(int p : nonShareable)
        {
            pluginLocks_[p] = std::make_unique<std::mutex>();
        }
    }

    void Executor::insertTransactionQueue(DriverKey, TransactionQueue* txnQueue)
    {
        txnQueue_ = txnQueue;
    }

    void Executor::insertListener(DriverKey, IListener* listener)
    {
        listener_ = listener;
    }

    void Executor::start(DriverKey)
    {
        if(optimized_)
        {
            for(int i = 0; i < threadNum_; i++)
            {
                txnQueues_.emplace_back(std::make_unique<TransactionQueue>());
                states_.emplace_back(std::make_unique<ExecutorState>(txnQueues_[i].get()));
                states_[i]->started = true;
                threads_.emplace_back(std::thread([this, state = states_[i].get()]()
                {
                    this->run(state);
                }));
            }
        }
        else
        {
            for(int i = 0; i < threadNum_; i++)
            {
                states_.emplace_back(std::make_unique<ExecutorState>(txnQueue_));
                states_[i]->started = true;
                threads_.emplace_back(std::thread([this, state = states_[i].get()]()
                {
                    this->run(state);
                }));
            }
        }
    }

    void Executor::assignTransaction(SchedulerKey, std::unique_ptr<ITransaction> txn, int index)
    {
        states_[index]->txnQueue->push(move(txn));
        states_[index]->running = true;
    }

    void Executor::stop(DriverKey)
    {
        for(int i = 0; i < threadNum_; i++)
        {
            states_[i]->started = false;
            states_[i]->txnQueue->exit();
            threads_[i].join();
        }
    }

    std::map<int, std::map<int,double>> Executor::getStats(DriverKey)
    {
        std::map<int, std::map<int,double>> res;
        std::map<int, std::map<int,int>> count;

        for(int i = 0; i < threadNum_; i++)
        {
            for(auto p1 : states_[i]->totalTimes)
            {
                for(auto p2 : p1.second)
                {
                    res[p1.first][p2.first] += p2.second;
                    count[p1.first][p2.first] += states_[i]->counts.at(p1.first).at(p2.first);
                }
            }
        }

        for(auto p1 : res)
        {
            for(auto p2 : p1.second)
            {
                res[p1.first][p2.first] = res.at(p1.first).at(p2.first) / count.at(p1.first).at(p2.first);                
            }
        }

        states_.clear();        
        return res;
    }
}