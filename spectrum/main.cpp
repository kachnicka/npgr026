#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "SpectralData.h"

class InputParser
{
public:
    InputParser (int &argc, char **argv)
    {
        for (int i=1; i < argc; ++i)
            tokens.emplace_back(argv[i]);
    }

    [[nodiscard]] const std::string& getCmdOption(const std::string& option) const
    {
        auto itr =  std::find(tokens.cbegin(), tokens.cend(), option);
        if (itr != tokens.cend() && ++itr != tokens.end())
            return *itr;
        return emptyString;
    }

    [[nodiscard]] bool cmdOptionExists(const std::string& option) const
    {
        return std::find(tokens.cbegin(), tokens.cend(), option) != tokens.cend();
    }
private:
    std::vector <std::string> tokens;
    std::string emptyString = {};
};

struct AppState
{
    int randomSampleCount = 100;
    int equidistantSampleCount = 40;

    void print() const
    {
        std::cout << "Random sample count: " << randomSampleCount << "\n";
        std::cout << "Equidistant sample count: " << equidistantSampleCount << "\n";
    }
};

class Spectrum
{
    static constexpr int visibleLow = 380;
    static constexpr int visibleHigh = 730;
    std::array<float, visibleHigh - visibleLow> spectrum = {};
};


int main(int argc, char **argv)
{
    InputParser input(argc, argv);
    if (input.cmdOptionExists("-h") || input.cmdOptionExists("--help"))
    {
        std::cout << "spectrum [-n RANDOM_SAMPLE_COUNT] [-m EQUIDISTANT_SAMPLE_COUNT]\n";
        return EXIT_SUCCESS;
    }

    AppState state;
    if (auto o = input.getCmdOption("-n"); !o.empty())
        state.randomSampleCount = std::stoi(o);
    if (auto o = input.getCmdOption("-m"); !o.empty())
        state.equidistantSampleCount = std::stoi(o);

    state.print();

    std::cout << Data::CIE_X << '\n';

    return EXIT_SUCCESS;
}
