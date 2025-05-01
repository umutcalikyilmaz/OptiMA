#include <Engine/DefaultEstimator.h>

namespace OptiMA
{
    DefaultEstimator::DefaultEstimator(string statsFilePath)
    {
        fstream file;
        file.open(statsFilePath, fstream::in);
        string line;        

        while(getline(file, line))
        {
            int size = line.size();
            int pos = 0;
            int st = 0;
            vector<int> keys;
            double value;

            while(pos < size)
            {
                if(line.at(pos) == ',')
                {
                    keys.push_back(stoi(line.substr(st, pos - st)));
                    st = pos + 1;
                }
                else if(pos == size - 1)
                {
                    value = stod(line.substr(st, size - st));
                }

                pos++;
            }

            if(value <= 0)
            {
                throw InvalidModelParameterException((char*)"Transaction length must be greater than 0");
            }

            averages[keys[0]][keys[1]][keys[2]] = value;
        }

        file.close();
    }

    double DefaultEstimator::EstimateLength(const ITransaction& txn)
    {
        int a = txn.GetAgentType();
        TransactionType t = txn.GetTransactionType();
        int st = txn.GetSubType();
        if(txn.GetTransactionType() == HALTPROGRAM)
        {
            return 1;
        }
        
        try
        {
            return averages.at(txn.GetAgentType()).at(txn.GetTransactionType()).at(txn.GetSubType());
        }
        catch(exception e)
        {
            return 10000;
        }
        
    }
}