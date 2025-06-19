#include "OptiMA/Engine/Executor.h"

namespace OptiMA
{
    void Executor::lockPlugins(const set<int>& plugins)
    {
        for(auto p : plugins)
        {
            pluginLocks_.at(p)->lock();
        }
    }

    void Executor::unlockPlugins(const set<int>& plugins)
    {
        for(auto p : plugins)
        {
            pluginLocks_.at(p)->unlock();
        }
    }

    void Executor::executeTransaction(unique_ptr<ITransaction> txn, ExecutorState* state)
    {
        if(txn == nullptr)
        {
            return;
        }

        const set<int> nonShareablePlugins = txn->getNonShareblePlugins();        

        lockPlugins(nonShareablePlugins);

        double beg = chrono::steady_clock::now().time_since_epoch().count();
        shared_ptr<TransactionResult> res = txn->execute();

        if(keepStats_)
        {            
            double end = chrono::steady_clock::now().time_since_epoch().count();            
            state->totalTimes[txn->getType()][txn->getSubType()] += end - beg;
            state->counts[txn->getType()][txn->getSubType()]++;
        }

        unlockPlugins(nonShareablePlugins);

        tfactory_->postProcess(move(txn), res);        
    }

    void Executor::run(ExecutorState* state)
    {
        while(state->started)
        {
            unique_ptr<ITransaction> txn = state->txnQueue->pull();

            if(trigger_ && state->txnQueue->isEmpty())
            {
                listener_->trigger();
            }

            executeTransaction(move(txn), state);            
        }
    }

    Executor::Executor(IDriver* driver, TransactionFactory* tfactory, PluginManager* pmanager, int threadNum,
    const set<int>& nonshareablePlugins, bool optimized, bool trigger, bool keepStats) : driver_(driver), tfactory_(tfactory),
    pmanager_(pmanager), lock_(false), threadNum_(threadNum), optimized_(optimized), trigger_(trigger), keepStats_(keepStats)
    {
        states_ = new ExecutorState*[threadNum];
        threads_ = new thread[threadNum];
        set<int> nonShareable = pmanager_->getNonShareable();

        for(int p : nonShareable)
        {
            pluginLocks_[p] = make_unique<mutex>();
        }
    }

    void Executor::insertTransactionQueue(TransactionQueue* txnQueue)
    {
        txnQueue_ = txnQueue;
    }

    void Executor::insertListener(IListener* listener)
    {
        listener_ = listener;
    }

    void Executor::start()
    {
        if(optimized_)
        {
            for(int i = 0; i < threadNum_; i++)
            {
                states_[i] = new ExecutorState();
                states_[i]->started = true;
                threads_[i] = thread([this, state = states_[i]]()
                {
                    this->run(state);
                });
            }
        }
        else
        {
            for(int i = 0; i < threadNum_; i++)
            {
                states_[i] = new ExecutorState(txnQueue_);
                states_[i]->started = true;
                threads_[i] = thread([this, state = states_[i]]()
                {
                    this->run(state);
                });
            }
        }
    }

    void Executor::assignTransaction(unique_ptr<ITransaction> txn, int index)
    {
        states_[index]->txnQueue->push(move(txn));
        states_[index]->running = true;
    }

    void Executor::stop()
    {
        for(int i = 0; i < threadNum_; i++)
        {
            states_[i]->started = false;
            states_[i]->txnQueue->exit();
            threads_[i].join();
        }
    }

    map<int,map<int,double>> Executor::getStats()
    {
        map<int,map<int,double>> res;
        map<int,map<int,int>> count;

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

        for(int i = 0; i < threadNum_; i++)
        {
            delete states_[i];
        }
        
        return res;
    }

    Executor::~Executor()
    {
        delete[] states_;
        delete[] threads_;
    }
}