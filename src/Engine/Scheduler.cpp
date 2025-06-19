#include "OptiMA/Engine/Scheduler.h"

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

    void Scheduler::findConflicts()
    {        
        txnNum_ = txns_.size();
        map<int,vector<int>> pluginUse;
        conflicts_ = new bool*[txnNum_];

        for(int i = 0; i < txnNum_; i++)
        {
            conflicts_[i] = new bool[txnNum_];

            for(int j = 0; j < txnNum_; j++)
            {
                conflicts_[i][j] = false;
            }
        }

        for(int nsp : nonShareablePlugins_)
        {
            pluginUse[nsp] = vector<int>();
        }

        for(int i = 0; i < txnNum_; i++)
        {
            auto nonShareable = txns_[i]->getNonShareblePlugins();

            for(int nsp : nonShareable)
            {
                pluginUse[nsp].push_back(i);
            }
        }

        for(int i = 0; i < txnNum_; i++)
        {
            auto nonShareable = txns_[i]->getNonShareblePlugins();

            for(int nsp : nonShareable)
            {
                for(int t : pluginUse[nsp])
                {
                    conflicts_[i][t] = true;
                }
            }
        }
    }

    void Scheduler::createPlan(TxnSP::SolverOutput*& out)
    {
        lengths_ = new double[txnNum_];

        for(int i = 0; i < txnNum_; i++)
        {
            lengths_[i] = txns_[i]->getLength();
        }

        sinp_.prb = new TxnSP::Problem(txns_.size(), threadNum_, lengths_, conflicts_);
        out = slv_->solve(sinp_);
        delete sinp_.prb;
    }

    Scheduler::Scheduler(SchedulerSettings* settings, IExecutor* executor, const set<int>& nonShareablePlugins,
    int threadNum) : executor_(executor), nonShareablePlugins_(nonShareablePlugins), threadNum_(threadNum), running_(false),
    stopped_(true)
    {
        perm_ = settings->permuted;
        optimized_ = settings->optimized;

        switch (settings->optimizationMethod)
        {
        case TxnSP::DP :
            slv_ = new TxnSP::DPSolver();
            sinp_.DP_SolutionType = settings->DP_SolutionType;
            break;

        case TxnSP::ES :
            slv_ = new TxnSP::ESSolver();
            break;

        #ifdef ENABLE_MIP
        case TxnSP::MIP :
            slv_ = new TxnSP::MIPSolver();
            break;
        #endif

        case TxnSP::SA :
            slv_ = new TxnSP::SASolver();
            sinp_.SA_DecrementParameter = settings->SA_DecrementParameter;
            sinp_.SA_DecrementType = settings->SA_DecrementType;
            sinp_.SA_MaxTemperature = settings->SA_MaxTemperature;
            break;
        }

        if(settings->permuted)
        {
            optimizePtr_ = &Scheduler::permutation;
            order_ = new int[threadNum];
            norder_ = new int[threadNum];
            total_ = new double[threadNum];

            for(int i = 0; i < threadNum; i++)
            {
                order_[i] = i;
                total_[i] = 0;
            }
        }
        else
        {
            optimizePtr_ = &Scheduler::noPermutation;
        }       
    }

    void Scheduler::optimize(TxnSP::SolverOutput*& out)
    {
        txns_ = txnQueue_->pullAll();
        findConflicts();
        createPlan(out);
    }

    void Scheduler::noPermutation()
    {
        while(running_)
        {            
            TxnSP::SolverOutput* out;
            optimize(out);

            for(int i = 0; i < threadNum_; i++)
            {
                for(int job : out->jobs[i])
                {
                    executor_->assignTransaction(move(txns_[job]), i);
                }
            }

            delete out;
            txns_.clear();
        }

        stopped_ = true;
        cv_.notify_one();
    }

    void Scheduler::permutation()
    {
        while(running_)
        {
            TxnSP::SolverOutput* out;
            optimize(out);

            OrderDescending(norder_, out->processingTimes, threadNum_);
                
            for(int i = 0; i < threadNum_; i++)
            {
                vector<ITransaction*> transactionList;

                for(int job : out->jobs[norder_[i]])
                {
                    executor_->assignTransaction(move(txns_[job]), order_[i]);
                }

                total_[order_[i]] += out->processingTimes[norder_[i]];
            }
                
            OrderAscending(order_, total_, threadNum_);
            delete out;
            txns_.clear();
        }

        stopped_ = true;
        cv_.notify_one();
    }

    void Scheduler::insertTransactionQueue(TransactionQueue* txnQueue)
    {
        running_ = true;
        txnQueue_ = txnQueue;
    }

    void Scheduler::startScheduling()
    {
        running_ = true;
        stopped_ = false;
        (this->*optimizePtr_)();
    }

    Scheduler::~Scheduler()
    {
        running_ = false;
        txnQueue_->trigger();
        unique_lock<mutex> lock(deleteLock_);
        cv_.wait(lock, [this] 
        {
            return stopped_.load();
        });

        delete slv_;

        if(perm_)
        {
            delete[] total_;
            delete[] order_;
            delete[] norder_;
        }
    }
}