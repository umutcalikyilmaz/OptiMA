#include <Structs/SolverOutput.h>

namespace TransactionScheduling
{
	int Comp(const void* a, const void* b)
	{
		if ( *(double*)a < *(double*)b ) 
		{
			return -1;
		}	
		else if ( *(double*)a > *(double*)b )
		{
			return 1;
		} 
		else
		{
			return 0;
		} 
	}

	SolverOutput::SolverOutput(Problem* prb, double runtime)
	{
		n = prb->GetN();
		m = prb->GetM();
		makespan = 0;
		mintime = DBL_MAX;
		cores = vector<vector<int>>(m);
		coreTimes = vector<double>(m);
		startingTimes = vector<double>(n);
		endingTimes = vector<double>(n);
		assignments = vector<int>(n);
		conflicts = vector<vector<int>>(n);
		double* t = prb->GetT();
		bool** conf = prb->GetConf();

		for(int i = 0; i < n; i++)
		{
			cores[i].push_back(i);
			double temp = 0;

			for(int j = 0; j < i; j++)
			{
				if(conf[i][j] && temp < coreTimes[j])
				{
					temp = coreTimes[j];
				}
			}

			startingTimes.push_back(temp);
			endingTimes.push_back(temp + t[i]);
			coreTimes.push_back(endingTimes[i]);
			assignments.push_back(i);
			
			if(makespan < coreTimes[i])
			{
				makespan = coreTimes[i];
			}

			if(mintime > coreTimes[i])
			{
				mintime = coreTimes[i];
			}
		}

		for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				if(conf[i][j] && startingTimes[j] < startingTimes[i])
				{
					conflicts[i].push_back(j);
				}
			}
		}
	}

	SolverOutput::SolverOutput(Problem* prb, int* state, double runtime)
	{
		n = prb->GetN();
		m = prb->GetM();        
        cores = vector<vector<int>>(m);
		coreTimes = vector<double>(m);
		startingTimes = vector<double>(n);
		endingTimes = vector<double>(n);
		assignments = vector<int>(n);
		conflicts = vector<vector<int>>(n);
		double* T = prb->GetT();
        bool** conf = prb->GetConf();
		int* coreNums = new int[m];
		int* lastJobs = new int[m];
		int* order = new int[m];

		double temp;
		int job;

		for(int i = 0; i < n; i++)
		{
			startingTimes.push_back(0);
			endingTimes.push_back(0);
		}

		for (int i = 0; i < m; i++)
		{
			temp = 0;
			job = state[i];
			cores[i].push_back(job);
			lastJobs[i] = job;
			coreNums[i] = 1;
			
			for (int j = i - 1; j > -1; j--)
			{
				if (conf[job][lastJobs[order[j]]])
				{
					temp = coreTimes[order[j]];
					break;
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			coreTimes.push_back(endingTimes[job]);
			int ind = FindPlace2(coreTimes, order, coreTimes[i], i);
			Shift(order, ind, i);
			order[ind] = i;
		}

		for(int i = m; i < n; i++)
		{
			temp = coreTimes[order[0]];
			job = state[i];

			cores[order[0]].push_back(job);
			lastJobs[order[0]] = job;
			coreNums[order[0]]++;

			for (int j = m - 1; j > 0; j--)
			{
				if (conf[job][lastJobs[order[j]]])
				{
					temp = coreTimes[order[j]];
					break;
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			coreTimes[order[0]] = endingTimes[job];
			int ind = FindPlace(coreTimes, order, coreTimes[order[0]], m);
			int tempi = order[0];
			Shift(order, ind);
			order[ind] = tempi;
		}

		mintime = coreTimes[order[0]];
		makespan = coreTimes[order[m - 1]];

		for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				if(conf[i][j] && startingTimes[j] < startingTimes[i])
				{
					conflicts[i].push_back(j);
				}
			}
		}
	}
	
    SolverOutput::SolverOutput(Problem* prb, Schedule* sch, double runtime) : runtime(runtime)
    {
        n = sch->GetN();
		m = sch->GetM();
        makespan = sch->GetMakespan();
		mintime = sch->GetMintime();
        cores = vector<vector<int>>(m);
		coreTimes = vector<double>(m);
		startingTimes = vector<double>(n);
		endingTimes = vector<double>(n);
		assignments = vector<int>(n);
		conflicts = vector<vector<int>>(n);
        int* coreCount = new int[m];
		int* tempLast = new int[m];
        int* coreNums = sch->GetCoreNums();
        double* ccoreTimes = sch->GetCoreTimes();
        int** jobs = sch->GetJobs();
        bool** conf = prb->GetConf();
		double* T = prb->GetT();
		double temp;	
		int job;

		//std::cout << "Begin\n";

		for(int i = 0; i < m; i++)
		{
			for(int j = 0; j < coreNums[i]; j++)
			{
				cores[i].push_back(jobs[i][j]);
			}

			job = jobs[i][0];
			temp = 0;

			for(int j = 0; j < i; j++)
			{
				if(conf[job][tempLast[j]] && temp < coreTimes[j])
				{
					temp = coreTimes[j];
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			coreTimes[i] = endingTimes[job];
			assignments[job] = i;
			tempLast[i] = job;
			coreCount[i] = 1;
		}

		int minCore = 0;
		temp = 1.7976931e+308;

		for(int i = 0; i < m; i++)
		{
			if(coreTimes[i] < temp)
			{
				minCore = i;
				temp = coreTimes[i];
			}
		}

		for(int i = m; i < n; i++)
		{			
			job = jobs[minCore][coreCount[minCore]];
			temp = coreTimes[minCore];

			for(int j = 0; j < m; j++)
			{
				if(conf[job][tempLast[j]] && temp < coreTimes[j])
				{
					temp = coreTimes[j];
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			coreTimes[minCore] = endingTimes[job];
			assignments[job] = minCore;
			tempLast[minCore] = job;
			coreCount[minCore]++;
			

			minCore = 0;
			temp = 1.7976931e+308;

			for(int j = 0; j < m; j++)
			{
				if(coreTimes[j] < temp)
				{
					minCore = j;
					temp = coreTimes[j];
				}
			}
		}

        delete[] coreCount;
		delete[] tempLast;

        for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				if(conf[i][j] && startingTimes[j] < startingTimes[i])
				{
					conflicts[i].push_back(j);
				}
			}
		}
    }
	
	SolverOutput::SolverOutput(Problem* prb, int* x, double* s, double runtime) : runtime(runtime)
	{
		n = prb->GetN();
        m = prb->GetM();
        cores = vector<vector<int>>(m);
		coreTimes = vector<double>(m);
		startingTimes = vector<double>(n);
		endingTimes = vector<double>(n);
		assignments = vector<int>(n);
		conflicts = vector<vector<int>>(n);
		double* T = prb->GetT();
        bool** conf = prb->GetConf();

		double* scpy = new double[n];
		int* ind = new int[n];
		bool* smt = new bool[n];
        int* lastJobs = new int[m];
        int minimumCore = 0;

		for (int i = 0; i < n; i++)
		{
			scpy[i] = s[i];
			smt[i] = true;
            startingTimes[i] = s[i];
            assignments[i] = x[i];
		}

		std::qsort(scpy, n, sizeof(double), Comp);

		for (int i = 0; i < n; i++)
		{	
			for (int j = 0; j < n; j++)
			{
				if (scpy[i] - s[j] < 0.0000001 && scpy[i] - s[j] > -0.0000001 && smt[j])
				{
					ind[i] = j;
					smt[j] = false;
					break;
				}
			}
		}

		for (int i = 0; i < n; i++)
		{
			cores[x[ind[i]]].push_back(ind[i]);

			for(int j = 0; j < n; j++)
			{
				if(conf[i][j])
				{
					conflicts[i].push_back(j);
				}
			}
		}

		makespan = 0;
		mintime = 1.79769310e+308;

		for (int i = 0; i < m; i++)
		{
			if (cores[i].size() > 0)
			{
				lastJobs[i] = cores[i].back();
				coreTimes[i] = s[lastJobs[i]] + T[lastJobs[i]];
			}
			else
			{
				lastJobs[i] = -1;
				coreTimes[i] = 0;
			}

			if (coreTimes[i] <= mintime)
			{
				mintime = coreTimes[i];
				minimumCore = i;
			}

			if (coreTimes[i] > makespan)
			{
				makespan = coreTimes[i];
			}
		}

		delete[] scpy;
		delete[] ind;
		delete[] smt;
		delete[] lastJobs;
	}

	SolverOutput::~SolverOutput() { }
}