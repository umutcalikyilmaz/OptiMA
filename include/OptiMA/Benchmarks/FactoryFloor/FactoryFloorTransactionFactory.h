#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/Assemble.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/CheckTime.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/GetJobDescription.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/InsertJobDescriptions.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/ManageConveyorBelt.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/ManageInspectionQueue.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/PickUpFromConveyorBelt.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/PickUpFromInspectionQueue.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/PlaceOnConveyorBelt.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/PlaceOnInspectionQueue.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/ReportResult.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/ScanPart.h"
#include "OptiMA/Benchmarks/FactoryFloor/Transactions/Traverse.h"
#include "OptiMA/Engine/TransactionFactory.h"

class FactoryFloorTransactionFactory : public TransactionFactory 
{
private:

    int initialAssemblyWorker_;
    int initialTransporter_;
    int initialInspector_;
    bool transporterInit_;
    bool inspectorInit_;

    vector<unique_ptr<ITransaction>> create0(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;
        
        if(get<0>(result->resultParameters->getTuple<bool>(0)))
        {
            auto resultTuple = result->resultParameters->getTuple<int,vector<pair<OperationType, int>>,set<int>>(1);
            res.push_back(make_unique<Assemble>(txn->getSeizedAgents()[0], get<0>(resultTuple), get<1>(resultTuple), get<2>(resultTuple)));
        }
        else
        {
            res.push_back(make_unique<GetJobDescription>(txn->getSeizedAgents()[0]));
        }

        return res;
    }

    vector<unique_ptr<ITransaction>> create1(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;        

        if(get<0>(result->resultParameters->getTuple<bool>(0)))
        {
            auto resultTuple = result->resultParameters->getTuple<int,vector<pair<OperationType, int>>,set<int>>(1);
            res.push_back(make_unique<Assemble>(txn->getSeizedAgents()[0], get<0>(resultTuple), get<1>(resultTuple), get<2>(resultTuple)));
        }
        else
        {
            res.push_back(make_unique<PlaceOnConveyorBelt>(txn->getSeizedAgents()[0]));
        }

        return res;
    }

    vector<unique_ptr<ITransaction>> create2(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;

        if(get<0>(result->resultParameters->getTuple<bool>(0)))
        {
            res.push_back(make_unique<GetJobDescription>(txn->getSeizedAgents()[0]));
        }

        if(transporterInit_)
        {
            for(int i = 0; i < initialTransporter_; i++)
            {
                res.push_back(make_unique<PickUpFromConveyorBelt>());
            }

            transporterInit_ = false;
        }

        return res;
    }

    vector<unique_ptr<ITransaction>> create3(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;

        if(get<0>(result->resultParameters->getTuple<bool>(0)))
        {
            res.push_back(make_unique<Traverse>(txn->getSeizedAgents()[0], false));
        }
        else
        {
            res.push_back(make_unique<PickUpFromConveyorBelt>(txn->getSeizedAgents()[0]));
        }

        return res;
    }

    vector<unique_ptr<ITransaction>> create4(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;
        auto resultTuple = result->resultParameters->getTuple<bool,bool>(0);
        
        if(get<0>(resultTuple))
        {
            if(get<1>(resultTuple))
            {
                res.push_back(make_unique<PickUpFromConveyorBelt>(txn->getSeizedAgents()[0]));
            }           
        }
        else
        {
            res.push_back(make_unique<PlaceOnInspectionQueue>(txn->getSeizedAgents()[0]));
        }

        return res;
    }

    vector<unique_ptr<ITransaction>> create5(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;
        res.push_back(make_unique<Traverse>(txn->getSeizedAgents()[0], true));

        if(inspectorInit_)
        {
            for(int i = 0; i < initialInspector_; i++)
            {
                res.push_back(make_unique<PickUpFromInspectionQueue>());
            }

            res.push_back(make_unique<CheckTime>());
            inspectorInit_ = false;
        }

        return res;
    }

    vector<unique_ptr<ITransaction>> create6(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;

        if(get<0>(result->resultParameters->getTuple<bool>(0)))
        {
            res.push_back(make_unique<ScanPart>(txn->getSeizedAgents()[0]));            
        }
        else
        {
            res.push_back(make_unique<PickUpFromInspectionQueue>(txn->getSeizedAgents()[0]));
        }

        return res;
    }

    vector<unique_ptr<ITransaction>> create7(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;
        res.push_back(make_unique<ReportResult>(txn->getSeizedAgents()[0]));
        return res;
    }

    vector<unique_ptr<ITransaction>> create8(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;

        if(get<0>(result->resultParameters->getTuple<bool>(0)))
        {
            res.push_back(make_unique<PickUpFromInspectionQueue>(txn->getSeizedAgents()[0]));
        }

        return res;
    }

    vector<unique_ptr<ITransaction>> create9(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;

        for(int i = 0; i < initialAssemblyWorker_; i++)
        {
            res.push_back(make_unique<GetJobDescription>());
        }

        return res;
    }

    vector<unique_ptr<ITransaction>> create10(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;
        res.push_back(make_unique<ManageInspectionQueue>(txn->getSeizedAgents()[0]));
        return res;
    }

    vector<unique_ptr<ITransaction>> create11(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;
        res.push_back(make_unique<CheckTime>(txn->getSeizedAgents()[0]));
        return res;
    }

    vector<unique_ptr<ITransaction>> create12(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> res;

        if(get<0>(result->resultParameters->getTuple<bool>(0)))
        {
            res.push_back(make_unique<ManageConveyorBelt>(txn->getSeizedAgents()[0]));
        }
        else
        {
            res.push_back(make_unique<CheckTime>(txn->getSeizedAgents()[0]));
        }

        return res;
    }

public:

    FactoryFloorTransactionFactory(int initialAssemblyWorker, int initialTransporter, int initialInspector) : initialAssemblyWorker_(initialAssemblyWorker),
    initialTransporter_(initialTransporter), initialInspector_(initialInspector), transporterInit_(true), inspectorInit_(true) { }

    vector<unique_ptr<ITransaction>> generateInitialTransactions() override
    {
        vector<unique_ptr<ITransaction>> res;
        res.push_back(make_unique<InsertJobDescriptions>());
        return res;
    }

    vector<unique_ptr<ITransaction>> generateTransactions(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> previousResult) override
    {
        int transactionType = txn->getType();

        switch (transactionType)
        {
        case 0:
            return create0(move(txn), previousResult);
        
        case 1:
            return create1(move(txn), previousResult);

        case 2:
            return create2(move(txn), previousResult);
        
        case 3:
            return create3(move(txn), previousResult);
        
        case 4:
            return create4(move(txn), previousResult);
        
        case 5:
            return create5(move(txn), previousResult);

        case 6:
            return create6(move(txn), previousResult);
        
        case 7:
            return create7(move(txn), previousResult);

        case 8:
            return create8(move(txn), previousResult);
        
        case 9:
            return create9(move(txn), previousResult);

        case 10:
            return create10(move(txn), previousResult);
        
        case 11:
            return create11(move(txn), previousResult);

        case 12:
            return create12(move(txn), previousResult);
        }
    }
};
