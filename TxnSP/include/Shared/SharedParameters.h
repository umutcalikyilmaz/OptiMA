#pragma once

namespace TransactionScheduling
{
    enum ProbabilityDistribution
	{
		UniformDistribution,
		NormalDistribution
	};

	enum SolutionType
	{
		Approximate,
		Exact
	};

	enum TemperatureEvolution
	{
		Exponential,
		Linear,
		Slow
	};

	enum SolverType
	{
		DP,
		ES,
		MIP,
		SA
	};
}