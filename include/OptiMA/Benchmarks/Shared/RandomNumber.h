#pragma once
#include <random>
#include <chrono>

class UniformRandom
{
    std::minstd_rand generator_;
    std::uniform_real_distribution<double> uniformDistribution_;

public:
    UniformRandom(double min, double max) : uniformDistribution_(min, max), generator_(static_cast<unsigned>(
    std::chrono::system_clock::now().time_since_epoch().count())) { }

    UniformRandom(double min, double max, unsigned seed) : uniformDistribution_(min, max), generator_(seed) { }

    double generate()
    {
        return uniformDistribution_(generator_);
    }
};


class NormalRandom
{
    std::minstd_rand generator_;
    std::normal_distribution<double> normalDistribution_;

public:
    NormalRandom(double min, double max) : normalDistribution_(min, max), generator_(static_cast<unsigned>(
    std::chrono::system_clock::now().time_since_epoch().count())) { }

    NormalRandom(double min, double max, unsigned seed) : normalDistribution_(min, max), generator_(seed) { } 

    double generate()
    {
        double res;
        do
        {
            res = normalDistribution_(generator_);
        }
        while (res <= 0);
    
        return res;
    }
};
