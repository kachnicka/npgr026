#pragma once

#include <chrono>
#include <random>

#include "Spectrum.h"

namespace Sampler
{
    struct RandomGenerator
    {
        explicit RandomGenerator(std::mt19937::result_type seed = randomSeed()) : gen(seed) {}
        std::mt19937 gen;
    private:
        static std::mt19937::result_type randomSeed()
        {
            const auto sEpoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            const auto nsEpoch = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            std::random_device rd;
            return rd() ^ (static_cast<std::mt19937::result_type>(sEpoch) + static_cast<std::mt19937::result_type>(nsEpoch));
        }
    };

    class Uniform
    {
    public:
        int getSample()
        {
            return distrib(g.gen);
        }

        [[nodiscard]] Spectrum::VisibleFull eval(int sampleCount, const Spectrum::VisibleFull& luminary, const Spectrum::VisibleFull& material)
        {
            Spectrum::VisibleFull result;
            for (auto i = 0; i < sampleCount; i++)
            {
                const auto lambda = getSample();

                // result should probably be average over multiple samples, which in this simple case equals to skipping already sampled wavelength
                if (result[lambda] == 0.f)
                    result[lambda] = luminary[lambda] * material[lambda];
            }
            return result;
        }

    private:
        RandomGenerator g;
        std::uniform_int_distribution<int> distrib{Spectrum::VisibleFull::LAMBDA_LOW, Spectrum::VisibleFull::LAMBDA_HIGH - 1};
    };

    using ivec4 = std::array<int, 4>;
    class Hero
    {
    public:
        ivec4 getSample()
        {
            auto s = distrib(g.gen);
            return { s + 0*Spectrum::VisibleFull::LAMBDA_HERO_STEP,
                     s + 1*Spectrum::VisibleFull::LAMBDA_HERO_STEP,
                     s + 2*Spectrum::VisibleFull::LAMBDA_HERO_STEP,
                     s + 3*Spectrum::VisibleFull::LAMBDA_HERO_STEP};
        }
    private:
        RandomGenerator g;
        // slightly distorted sampling due to defined visible wavelength range not being divisible by 4 - lambda 729 and 730 is never sampled
        std::uniform_int_distribution<int> distrib{Spectrum::VisibleFull::LAMBDA_LOW, Spectrum::VisibleFull::LAMBDA_LOW + Spectrum::VisibleFull::LAMBDA_HERO_STEP};
    };

    class Equidistant
    {
    public:
        static std::vector<int> getSample(int sampleCount)
        {
            const auto step = Spectrum::VisibleFull::LAMBDA_RANGE / sampleCount;
            std::vector<int> samples;
            samples.reserve(sampleCount);
            for (auto i = 0; i < sampleCount; i++)
                samples.emplace_back(Spectrum::VisibleFull::LAMBDA_LOW + i * step);

            assert(samples.size() == sampleCount);
            return samples;
        }
    };

}