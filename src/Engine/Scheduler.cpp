#include <Engine/Scheduler.h>

namespace OptiMA
{
    void Reorder(int* list, int size, int ind)
    {
        for(int i = size; i > ind; i--)
        {
            list[i] = list[i - 1];
        }
    }
    
	int findPlace(double* list, int* inds, double val, int size)
	{
		if(size == 0)
		{
			return 0;
		}
		
		int ul = size;
		int ll = 0; 
		int diff = ul - ll;		

		while(diff > 1)
		{
			int curr = diff / 2 + ll;

			if(val < list[inds[curr]])
			{
				ul = curr;
			}
			else if(val > list[inds[curr]])
			{
				ll = curr;
			}
			else
			{
				return curr;
			}

			diff = ul - ll;
		}

		if(val > list[inds[ll]])
		{
			return ul;
		}

		return ll;
	}

	int findPlace2(vector<double> list, int* inds, double val, int size)
	{
		if(size == 0)
		{
			return 0;
		}
		
		int ul = size;
		int ll = 0; 
		int diff = ul - ll;		

		while(diff > 1)
		{
			int curr = diff / 2 + ll;

			if(val > list[inds[curr]])
			{
				ul = curr;
			}
			else if(val < list[inds[curr]])
			{
				ll = curr;
			}
			else
			{
				return curr;
			}

			diff = ul - ll;
		}

		if(val < list[inds[ll]])
		{
			return ul;
		}

		return ll;
	}

    void OrderAscending(int* list, double* vlist, int m)
    {
        for(int i = 0; i < m; i++)
        {
            int ind = findPlace(vlist, list, vlist[i], i);
            Reorder(list, i, ind);
            list[ind] = i;
        }
    }

    void OrderDescending(int* list, vector<double> vlist, int m)
    {
        for(int i = 0; i < m; i++)
        {
            int ind = findPlace2(vlist, list, vlist[i], i);
            Reorder(list, i, ind);
            list[ind] = i;
        }
    }

    void Scheduler::FindConflicts()
    {        
        txnNum = txns.size();
        map<int,vector<int>> pluginUse;
        conflicts = new bool*[txnNum];

        for(int i = 0; i < txnNum; i++)
        {
            conflicts[i] = new bool[txnNum];

            for(int j = 0; j < txnNum; j++)
            {
                conflicts[i][j] = false;
            }
        }

        for(int nsp : nonShareablePlugins)
        {
            pluginUse[nsp] = vector<int>();
        }

        for(int i = 0; i < txnNum; i++)
        {
            auto nonShareable = txns[i]->GetNonShareblePlugins();

            for(int nsp : nonShareable)
            {
                pluginUse[nsp].push_back(i);
            }
        }

        for(int i = 0; i < txnNum; i++)
        {
            auto nonShareable = txns[i]->GetNonShareblePlugins();

            for(int nsp : nonShareable)
            {
                for(int t : pluginUse[nsp])
                {
                    conflicts[i][t] = true;
                }
            }
        }

        int asd = 0;
    }

    void Scheduler::CreatePlan(TransactionScheduling::SolverOutput*& out)
    {
        lengths = new double[txnNum];

        for(int i = 0; i < txnNum; i++)
        {
            lengths[i] = txns[i]->GetLength();
        }

        sinp.prb = new TransactionScheduling::Problem(txns.size(), threadNum, lengths, conflicts);
        out = slv->Solve(sinp);
        delete sinp.prb;
    }

    Scheduler::Scheduler(SchedulerSettings* settings, IExecutor* executor, const set<int>& nonShareablePlugins,
    int threadNum) : executor(executor), nonShareablePlugins(nonShareablePlugins), threadNum(threadNum), running(false),
    stopped(true)
    {
        perm = settings->permuted;
        optimized = settings->optimized;

        switch (settings->optimizationMethod)
        {
        case TransactionScheduling::DP :
            slv = new TransactionScheduling::DPSolver();
            sinp.DP_SolutionType = settings->DP_SolutionType;
            break;

        case TransactionScheduling::ES :
            slv = new TransactionScheduling::ESSolver();
            break;

        case TransactionScheduling::MIP :
            slv = new TransactionScheduling::MIPSolver();
            break;

        case TransactionScheduling::SA :
            slv = new TransactionScheduling::SASolver();
            sinp.SA_DecrementParameter = settings->SA_DecrementParameter;
            sinp.SA_DecrementType = settings->SA_DecrementType;
            sinp.SA_MaxTemperature = settings->SA_MaxTemperature;
            break;
        }

        if(settings->permuted)
        {
            optimizePtr = &Scheduler::Permutation;
            order = new int[threadNum];
            norder = new int[threadNum];
            total = new double[threadNum];

            for(int i = 0; i < threadNum; i++)
            {
                order[i] = i;
                total[i] = 0;
            }
        }
        else
        {
            optimizePtr = &Scheduler::NoPermutation;
        }       
    }

    void Scheduler::Optimize(TransactionScheduling::SolverOutput*& out)
    {
        txns = txnQueue->PullAll();
        FindConflicts();
        CreatePlan(out);
    }

    void Scheduler::NoPermutation()
    {
        while(running)
        {            
            TransactionScheduling::SolverOutput* out;
            Optimize(out);

            for(int i = 0; i < threadNum; i++)
            {
                for(int job : out->cores[i])
                {
                    executor->AssignTransaction(move(txns[job]), i);
                }
            }

            delete out;
            txns.clear();
        }

        stopped = true;
        cv.notify_one();
    }

    void Scheduler::Permutation()
    {
        while(running)
        {
            TransactionScheduling::SolverOutput* out;
            Optimize(out);

            OrderDescending(norder, out->coreTimes, threadNum);
                
            for(int i = 0; i < threadNum; i++)
            {
                vector<Transaction*> transactionList;

                for(int job : out->cores[norder[i]])
                {
                    executor->AssignTransaction(move(txns[job]), order[i]);
                }

                total[order[i]] += out->coreTimes[norder[i]];
            }
                
            OrderAscending(order, total, threadNum);
            delete out;
            txns.clear();
        }

        stopped = true;
        cv.notify_one();
    }

    void Scheduler::InsertTransactionQueue(TransactionQueue* txnQueue)
    {
        running = true;
        this->txnQueue = txnQueue;
    }

    void Scheduler::StartScheduling()
    {
        running = true;
        stopped = false;
        (this->*optimizePtr)();
    }

    Scheduler::~Scheduler()
    {
        running = false;
        txnQueue->Trigger();
        unique_lock<mutex> lock(deleteLock);
        cv.wait(lock, [this] 
        {
            return stopped.load();
        });

        delete slv;

        if(perm)
        {
            delete[] total;
            delete[] order;
            delete[] norder;
        }
    }
}