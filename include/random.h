#ifndef RANDOM_H
#define RANDOM_H

#include <random>

namespace Random
{
  static std::random_device rd;
  static std::mt19937 gen(rd());

  static int randomInt(int min = 0, int max = 2147483647)
  {
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
  }

  static double randomDouble(double min = 0.0, double max = 1.0)
  {
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
  }

  static bool randomBool()
  {
    std::uniform_int_distribution<> dis(0, 1);
    return dis(gen) == 1;
  }

  static int poisson(double mean)
  {
    std::poisson_distribution<> dis(mean);
    return dis(gen);
  }

  static double exponential(double mean)
  {
    std::exponential_distribution<> dis(mean);
    return dis(gen);
  }

  static int exponentialSamples(double mean, double time)
  {
    int samples = 0;
    double t = 0.0;
    while (t < time)
    {
      t += exponential(mean);
      samples++;
    }
    return samples - 1;
  }

};

#endif // !RANDOM_H