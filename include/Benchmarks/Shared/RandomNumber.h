#pragma once
#include <random>
#include <chrono>

class UniformRandom
{
    std::minstd_rand generator;
    std::uniform_real_distribution<double> uniformDistribution;

public:
    UniformRandom(double min, double max) : uniformDistribution(min, max), generator(static_cast<unsigned>(
    std::chrono::system_clock::now().time_since_epoch().count())) { }

    UniformRandom(double min, double max, unsigned seed) : uniformDistribution(min, max), generator(seed) { }

    double Generate()
    {
        return uniformDistribution(generator);
    }
};


class NormalRandom
{
    std::minstd_rand generator;
    std::normal_distribution<double> normalDistribution;

public:
    NormalRandom(double min, double max) : normalDistribution(min, max), generator(static_cast<unsigned>(
    std::chrono::system_clock::now().time_since_epoch().count())) { }

    NormalRandom(double min, double max, unsigned seed) : normalDistribution(min, max), generator(seed) { } 

    double Generate()
    {
        double res;
        do
        {
            res = normalDistribution(generator);
        }
        while (res <= 0);
    
        return res;
    }
};
