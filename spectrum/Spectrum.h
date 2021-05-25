#pragma once

namespace Spectrum
{
    class Internal
    {
    public:
        static constexpr int visibleLow = 380;
        static constexpr int visibleHigh = 731;
        static constexpr int visibleSize = visibleHigh - visibleLow;

        void print()
        {
            for (auto i = 0; i < visibleSize; i++)
            {
                if (i % 10 == 0)
                    std::cout << '\n';
                std::cout << "{" << visibleLow + i << ", " << spectrum[i] << "}\t";
            }
            std::cout << '\n';
        }
    private:
        std::array<float, visibleSize> spectrum = {};
    };

    using Sparse = std::unordered_map<int, float>;

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
        Sparse parseMathematicaString(const char* data)
        {
            reset();
            std::string s{data};
            Sparse spectrum;

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
                            spectrum[key] = value;
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
