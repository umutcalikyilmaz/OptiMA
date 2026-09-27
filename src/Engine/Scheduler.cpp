#include "OptiMA/Engine/Scheduler.h"

namespace OptiMA
{
    void reorder(std::vector<int> list, int size, int ind)
    {
        for(int i = size; i > ind; i--)
        {
            list[i] = list[i - 1];
        }
    }
    
	int findPlace(const std::vector<double>& list, std::vector<int>& inds, double val, int size)
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

	int findPlace2(const std::vector<double>& list, std::vector<int>& inds, double val, int size)
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

    void orderAscending(std::vector<int>& list, const std::vector<double>& vlist, int m)
    {
        for(int i = 0; i < m; i++)
        {
            int ind = findPlace(vlist, list, vlist[i], i);
            reorder(list, i, ind);
            list[ind] = i;
        }
    }

    void orderDescending(std::vector<int>& list, const std::vector<double>& vlist, int m)
    {
        for(int i = 0; i < m; i++)
        {
            int ind = findPlace2(vlist, list, vlist[i], i);
            reorder(list, i, ind);
            list[ind] = i;
        }
    }

    void Scheduler::findConflicts()
    {        
        txnNum_ = txns_.size();
        std::map<int ,std::vector<int>> pluginUse;
        conflicts_.resize(txnNum_);

        for(int i = 0; i < txnNum_; i++)
        {
            conflicts_[i].assign(txnNum_, 0);
        }

        for(int nsp : nonShareablePlugins_)
        {
            pluginUse[nsp] = std::vector<int>();
        }

        for(int i = 0; i < txnNum_; i++)
        {
            auto nonShareable = txns_[i]->getNonShareablePlugins();

            for(int nsp : nonShareable)
            {
                pluginUse[nsp].push_back(i);
            }
        }

        for(int i = 0; i < txnNum_; i++)
        {
            auto nonShareable = txns_[i]->getNonShareablePlugins();

            for(int nsp : nonShareable)
            {
                for(int t : pluginUse[nsp])
                {
                    conflicts_[i][t] = 1;
                }
            }
        }
    }

    TxnSP::SolverOutput Scheduler::createPlan()
    {
        lengths_.clear();
        lengths_.reserve(txnNum_);

        for(int i = 0; i < txnNum_; i++)
        {
            lengths_.push_back(txns_[i]->getLength());
        }

        auto prb = std::make_unique<TxnSP::Problem>(txns_.size(), threadNum_, lengths_, conflicts_);
        sinp_.prb = prb.get();
        return slv_->solve(sinp_);
    }

    Scheduler::Scheduler(DriverKey, SchedulerSettings* settings, IExecutor* executor,
        const std::set<int>& nonShareablePlugins, int threadNum)
        : executor_(executor),
          nonShareablePlugins_(nonShareablePlugins),
          threadNum_(threadNum),
          running_(false),
          stopped_(true)
    {
        optimized_ = settings->optimized;

        switch (settings->optimizationMethod)
        {
        case TxnSP::SolverType::DP :
            slv_ = std::make_unique<TxnSP::DPSolver>();
            sinp_.DP_SolutionType = settings->DP_SolutionType;
            break;

        case TxnSP::SolverType::ES :
            slv_ = std::make_unique<TxnSP::ESSolver>();
            break;

        #ifdef ENABLE_MIP
        case TxnSP::SolverType::MIP :
            slv_ = std::make_unique<TxnSP::MIPSolver>();
            break;
        #endif

        case TxnSP::SolverType::SA :
            slv_ = std::make_unique<TxnSP::SASolver>();
            sinp_.SA_DecrementParameter = settings->SA_DecrementParameter;
            sinp_.SA_DecrementType = settings->SA_DecrementType;
            sinp_.SA_MaxTemperature = settings->SA_MaxTemperature;
            break;
        }

        if(settings->permuted)
        {
            optimizePtr_ = &Scheduler::permutation;
            order_.reserve(threadNum);
            norder_.resize(threadNum);
            total_.assign(threadNum, 0);

            for(int i = 0; i < threadNum; i++)
            {
                order_.push_back(i);
            }
        }
        else
        {
            optimizePtr_ = &Scheduler::noPermutation;
        }       
    }

    void Scheduler::insertTransactionQueue(DriverKey, TransactionQueue* txnQueue)
    {
        running_ = true;
        txnQueue_ = txnQueue;
    }

    void Scheduler::startScheduling(DriverKey)
    {
        running_ = true;
        stopped_ = false;
        (this->*optimizePtr_)();
    }

    TxnSP::SolverOutput Scheduler::optimize()
    {
        txns_ = txnQueue_->pullAll();
        findConflicts();
        return createPlan();
    }

    void Scheduler::noPermutation()
    {
        while(running_)
        {            
            TxnSP::SolverOutput out = optimize();

            for(int i = 0; i < threadNum_; i++)
            {
                for(int job : out.jobs[i])
                {
                    executor_->assignTransaction(typename IExecutor::SchedulerKey {}, move(txns_[job]), i);
                }
            }

            txns_.clear();
        }

        stopped_ = true;
        cv_.notify_one();
    }

    void Scheduler::permutation()
    {
        while(running_)
        {
            TxnSP::SolverOutput out = optimize();
            orderDescending(norder_, out.processingTimes, threadNum_);
                
            for(int i = 0; i < threadNum_; i++)
            {
                for(int job : out.jobs[norder_[i]])
                {
                    executor_->assignTransaction(typename IExecutor::SchedulerKey {}, move(txns_[job]), order_[i]);
                }

                total_[order_[i]] += out.processingTimes[norder_[i]];
            }
                
            orderAscending(order_, total_, threadNum_);
            txns_.clear();
        }

        stopped_ = true;
        cv_.notify_one();
    }

    Scheduler::~Scheduler()
    {
        running_ = false;
        txnQueue_->trigger();
        std::unique_lock<std::mutex> lock(deleteLock_);
        cv_.wait(lock, [this] 
        {
            return stopped_.load();
        });
    }
}