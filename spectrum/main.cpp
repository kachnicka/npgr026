#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "SpectralData.h"
#include "Spectrum.h"

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

    Spectrum::Internal s;
    s.print();

    Spectrum::Parser p;
    auto ss = p.parseMathematicaString(Data::CIE_Illuminant_D65);

    std::cout << "{400, " << ss[400] << "}\n";
    std::cout << "{500, " << ss[500] << "}\n";


    return EXIT_SUCCESS;
}
