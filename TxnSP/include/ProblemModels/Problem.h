#pragma once
#include <cstdlib>
#include <chrono>
#include <math.h>
#include <Shared/SharedParameters.h>
#include <Shared/SharedModels.h>

using namespace std;

namespace TransactionScheduling
{
    class Problem
    {
        double* t;
        int* divid;
        bool** conf;
        int n;
		int m;
        __uint128_t size;

    public:
    
        Problem(int n, int m, double* t, bool** conf);

        Problem(int n, int m, double para1, double para2, double cp, ProbabilityDistribution dist);        	

		int GetN() const;
		
		int GetM() const;

		double* GetT() const;

        double GetT(int ind) const;

		bool** GetConf() const;

		int* GetDivid() const;

		__uint128_t GetSize() const;

        void ArrangeConf();

        ~Problem();
    };    
}