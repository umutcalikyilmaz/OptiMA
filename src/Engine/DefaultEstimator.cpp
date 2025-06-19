#include "OptiMA/Engine/DefaultEstimator.h"

namespace OptiMA
{
    DefaultEstimator::DefaultEstimator(string statsFilePath)
    {
        fstream file;
        file.open(statsFilePath, fstream::in);
        string line;
        generalAverage_ = 0;
        int count = 0;

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
                throw InvalidModelParameterException("Transaction length must be greater than 0");
            }

            averages_[keys[0]][keys[1]] = value;
            generalAverage_ += value;
            count++;
        }

        generalAverage_ /= count;
        file.close();
    }

    double DefaultEstimator::estimateLength(const ITransaction& txn)
    {      
        try
        {
            return averages_.at(txn.getType()).at(txn.getSubType());
        }
        catch(exception e)
        {
            return generalAverage_;
        }
    }
}