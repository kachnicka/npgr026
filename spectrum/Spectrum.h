#pragma once

#define DEBUG_LOG 0
#include <cassert>
#include <map>
#include <iostream>

namespace Spectrum
{
    class VisibleFull
    {
    public:
        static constexpr int LAMBDA_LOW = 380;
        static constexpr int LAMBDA_HIGH = 731;
        static constexpr int LAMBDA_RANGE = LAMBDA_HIGH - LAMBDA_LOW;
        static constexpr int LAMBDA_HERO_STEP = LAMBDA_RANGE / 4;

        void print() const
        {
            std::cout.setf(std::ios::fixed);
            std::cout.precision(4);
            for (auto i = 0; i < LAMBDA_RANGE; i++)
            {
                if (i % 10 == 0)
                    std::cout << '\n';
                std::cout << "{" << LAMBDA_LOW + i << ", " << values[i] << "}\t";
            }
            std::cout << '\n';
        }

        float& operator[](std::size_t idx)
        {
            assert(idx >= LAMBDA_LOW && idx < LAMBDA_HIGH);
            return values[idx - VisibleFull::LAMBDA_LOW];
        }

        const float& operator[](std::size_t idx) const
        {
            assert(idx >= LAMBDA_LOW && idx < LAMBDA_HIGH);
            return values[idx - VisibleFull::LAMBDA_LOW];
        }

        float sum() const
        {
            float sum = 0.f;
            for (int i = 0; i < LAMBDA_RANGE; i++)
                sum += values[i];
            return sum;
        }

        VisibleFull& operator*=(float scalar)
        {
            for (int i = 0; i < LAMBDA_RANGE; i++)
                values[i] *= scalar;
            return *this;
        }

        VisibleFull& operator*=(const VisibleFull& rhs)
        {
            for (int i = 0; i < LAMBDA_RANGE; i++)
                values[i] *= rhs.values[i];
            return *this;
        }

        friend VisibleFull operator*(VisibleFull lhs, const VisibleFull& rhs)
        {
            return lhs *= rhs;
        }
    private:
        std::array<float, LAMBDA_RANGE> values{};
    };

    class Arbitrary
    {
    public:
        std::map<int, float> values = {};

        [[nodiscard]] VisibleFull toVisibleFull() const
        {
            VisibleFull spectrum;
            for (auto l = VisibleFull::LAMBDA_LOW; l < VisibleFull::LAMBDA_HIGH; l++)
                spectrum[l] = lerpVal(l);
            return spectrum;
        }
    private:
        [[nodiscard]] float lerpVal(int lambda) const
        {
            if (auto val = values.find(lambda); val != values.end())
                return val->second;

            auto B = values.lower_bound(lambda);
            assert(B != values.end() && B != values.begin());
            const auto [B_lambda, B_value] = *B;

            auto A = B;
            --A;
            const auto [A_lambda, A_value] = *A;

            const auto range = static_cast<float>(B_lambda - A_lambda);
            const auto A_weight = static_cast<float>(B_lambda - lambda) / range;
            const auto B_weight = static_cast<float>(lambda - A_lambda) / range;

#if DEBUG_LOG
            std::cout << "<A> lambda: " << A_lambda << " weight: " << A_weight << " value: " << A_value << '\n';
            std::cout << "<B> lambda: " << B_lambda << " weight: " << B_weight << " value: " << B_value << '\n';
            std::cout << "LERP <" << lambda << "> = " << A_weight * A_value + B_weight * B_value << '\n';
#endif

            return A_weight * A_value + B_weight * B_value;
        }
    };


    class Parser
    {
        enum class State
        {
            eIdle,
            eParseKey,
            eParseValue
        } state = State::eIdle;
        std::string token;
        int key = 0;
        float value = 0.0f;

    public:
        Arbitrary parseMathematicaString(const char* data)
        {
            reset();
            std::string s{data};
            Arbitrary spectrum;

            for (const auto& c : s)
            {
                if (isWhitespace(c))
                    continue;
                switch (state)
                {
                    case State::eIdle:
                        if (c == '{') {
                            token.clear();
                            state = State::eParseKey;
                        }
                        break;
                    case State::eParseKey:
                        if (c == ',') {
                            key = std::stoi(token);
                            token.clear();
                            state = State::eParseValue;
                        } else
                            token.push_back(c);
                        break;
                    case State::eParseValue:
                        if (c == '}') {
                            value = std::stof(token);
                            token.clear();
                            spectrum.values[key] = value;
                            state = State::eIdle;
                        } else
                            token.push_back(c);
                        break;
                }
            }
            return spectrum;
        }
    private:
        void reset()
        {
            state = State::eIdle;
            token.clear();
            key = 0;
            value = 0.0f;
        }

        static bool isWhitespace(char c)
        {
            return c == '\n' || c == '\r' || c == '\t' || c == ' ';
        }
    };
}
