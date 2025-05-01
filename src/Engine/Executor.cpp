#include <Engine/Executor.h>
#include <iostream>

namespace OptiMA
{
    void Executor::LockPlugins(const set<int>& plugins)
    {
        for(auto p : plugins)
        {
            pluginLocks[p].lock();
        }
    }

    void Executor::UnlockPlugins(const set<int>& plugins)
    {
        for(auto p : plugins)
        {
            pluginLocks[p].unlock();
        }
    }

    void Executor::ExecuteTransaction(unique_ptr<ITransaction> txn, ExecutorState* state)
    {
        if(txn == nullptr)
        {
            return;
        }

        if(txn->GetTransactionType() == HALTPROGRAM)
        {
            int at = txn->GetAgentType();

            for(int a : haltingAgents)
            {
                if(a == at)
                {                    
                    driver->HaltProgram(txn->GetOutputParameters());
                    return;
                }
            }
        }
        int a = txn->GetAgentType();
        TransactionType tt = txn->GetTransactionType();
        int st = txn->GetSubType();

        if(a == 2 && tt == OPERATE, st == 1)
        {
            int rfr = 0;
        }

        const set<int> nonShareablePlugins = txn->GetNonShareblePlugins();

        

        LockPlugins(nonShareablePlugins);

        double beg = chrono::steady_clock::now().time_since_epoch().count();
        unique_ptr<ITransaction> res = txn->Execute();

        if(keepStats)
        {            
            double end = chrono::steady_clock::now().time_since_epoch().count();            
            state->totalTimes[txn->GetAgentType()][txn->GetTransactionType()][txn->GetSubType()] += end - beg;
            state->counts[txn->GetAgentType()][txn->GetTransactionType()][txn->GetSubType()]++;
        }

        UnlockPlugins(nonShareablePlugins);
        

        if(res != nullptr)
        {
            listener->SendTransaction(move(res));
        }
        
        //if(txn->GetTransactionType() != OPERATE && txn->GetTransactionType() != PROCESS)
        //cout << "Transaction type " << to_string(txn->GetTransactionType()) << "is executed.\n";
        //cout << to_string(count++) << "\n";
    }

    void Executor::Run(ExecutorState* state)
    {
        while(state->started)
        {
            unique_ptr<ITransaction> txn = state->txnQueue->Pull();

            if(state->txnQueue->isEmpty())
            {

            }

            ExecuteTransaction(move(txn), state);            
        }
    }

    Executor::Executor(IDriver* driver, PluginManager* pmanager, int threadNum, const set<int>& nonshareablePlugins,
    const vector<int>& haltingAgents, bool optimized, bool keepStats) : driver(driver), pmanager(pmanager),
    listener(listener), lock(false), threadNum(threadNum), optimized(optimized), keepStats(keepStats),
    haltingAgents(haltingAgents)
    {
        states = new ExecutorState*[threadNum];
        threads = new thread[threadNum];
    }

    void Executor::InsertTransactionQueue(TransactionQueue* txnQueue)
    {
        this->txnQueue = txnQueue;
    }

    void Executor::InsertListener(IListener* listener)
    {
        this->listener = listener;
    }

    void Executor::Start()
    {
        if(optimized)
        {
            for(int i = 0; i < threadNum; i++)
            {
                states[i] = new ExecutorState();
                states[i]->started = true;
                threads[i] = thread([this, state = states[i]]()
                {
                    this->Run(state);
                });
            }
        }
        else
        {
            for(int i = 0; i < threadNum; i++)
            {
                states[i] = new ExecutorState(txnQueue);
                states[i]->started = true;
                threads[i] = thread([this, state = states[i]]()
                {
                    this->Run(state);
                });
            }
        }
    }

    void Executor::AssignTransaction(unique_ptr<ITransaction> txn, int index)
    {
        states[index]->txnQueue->Push(move(txn));
        states[index]->running = true;
    }

    void Executor::Stop()
    {
        for(int i = 0; i < threadNum; i++)
        {
            states[i]->started = false;
            states[i]->txnQueue->Exit();
            threads[i].join();
        }
    }

    map<int,map<int,map<int,double>>> Executor::GetStats()
    {
        map<int,map<int,map<int,double>>> res;
        map<int,map<int,map<int,int>>> count;

        for(int i = 0; i < threadNum; i++)
        {
            for(auto p1 : states[i]->totalTimes)
            {
                for(auto p2 : p1.second)
                {
                    for(auto p3 : p2.second)
                    {
                        res[p1.first][p2.first][p3.first] += p3.second;
                        count[p1.first][p2.first][p3.first] += states[i]->counts.at(p1.first).at(p2.first).at(p3.first);
                    }
                }
            }
        }

        for(auto p1 : res)
        {
            for(auto p2 : p1.second)
            {
                for(auto p3 : p2.second)
                {
                    res[p1.first][p2.first][p3.first] /= count.at(p1.first).at(p2.first).at(p3.first);
                }
            }
        }
        

        return res;
    }

    Executor::~Executor()
    {
        for(int i = 0; i < threadNum; i++)
        {
            delete states[i];
        }
    }
}