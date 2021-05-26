#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "SpectralData.h"
#include "Spectrum.h"
#include "Sampler.h"
#include "Results.h"
#include "ColorSpace.h"

struct RunParams
{
    int randomSampleCount = 100;
    int equidistantSampleCount = 40;

    void print() const
    {
        std::cout << "Random sample count: " << randomSampleCount << "\n";
        std::cout << "Equidistant sample count: " << equidistantSampleCount << "\n";
    }
};

class SpectralMultiplication
{
public:
    SpectralMultiplication()
    {
        loadSpectralData();
    }

    void run(const RunParams& params) const
    {
        params.print();

        std::cout.setf(std::ios::fixed);
        std::cout.precision(2);

        std::cout << "\nUniform samples (" << params.randomSampleCount << "):\n";
        Sampler::Uniform uSampler;
        for (auto i = 0; i < params.randomSampleCount; i++)
            std::cout << uSampler.getSample() << "\t";
        std::cout << "\n";

        std::cout << "\nHero samples (" << params.randomSampleCount / 4 << "):\n";
        Sampler::Hero hSampler;
        for (auto i = 0; i < params.randomSampleCount / 4; i++)
        {
            const auto s = hSampler.getSample();
            for(const auto& j : s)
                std::cout << j << "\t";
        }
        std::cout << "\n";

        std::cout << "\nEquidistant samples (" << params.equidistantSampleCount << "):\n";
        const auto s = Sampler::Equidistant::getSample(params.equidistantSampleCount);
        for(const auto& j : s)
            std::cout << j << "\t";
        std::cout << "\n";

        Result r;
        for (const auto& [lumName, lumSpectrum] : luminary)
            for (const auto& [matName, matSpectrum] : material)
            {
                const auto evaluatedSpectrum = uSampler.eval(params.randomSampleCount, lumSpectrum, matSpectrum);
                const auto key = std::make_pair(lumName, matName);
                const auto rgb = ColorSpace::RGB(evaluatedSpectrum);
                r.values.emplace(key, rgb);
                std::cout << lumName << "\t" << matName << ":\t" << rgb.color.r << "\t" << rgb.color.g << "\t" << rgb.color.b << "\n";
            }

        std::cout << "\n";
        for (const auto& [lumName, lumSpectrum] : luminary)
            for (const auto& [matName, matSpectrum] : material)
            {

                const auto evaluatedSpectrum = lumSpectrum * matSpectrum;
                const auto key = std::make_pair(lumName, matName);
                const auto rgb = ColorSpace::RGB(evaluatedSpectrum);
                r.values.emplace(key, rgb);
                std::cout << lumName << "\t" << matName << ":\t" << rgb.color.r << "\t" << rgb.color.g << "\t" << rgb.color.b << "\n";
            }
    }

    void runDemo() const
    {
    }

    void printSpectralData() const
    {
        for(const auto& [name, spectrum] : luminary)
        {
            std::cout << "\n" << name;
            spectrum.print();
        }
        for(const auto& [name, spectrum] : material)
        {
            std::cout << "\n" << name;
            spectrum.print();
        }
    }

private:
    std::unordered_map<std::string, Spectrum::VisibleFull> luminary;
    std::unordered_map<std::string, Spectrum::VisibleFull> material;

    void loadSpectralData()
    {
        Spectrum::Parser p;
        // up-sampling using linear interpolation
        luminary["A"] = p.parseMathematicaString(Data::CIE_Illuminant_A).toVisibleFull();
        luminary["D65"] = p.parseMathematicaString(Data::CIE_Illuminant_D65).toVisibleFull();
        luminary["F11"] = p.parseMathematicaString(Data::CIE_Illuminant_F11).toVisibleFull();
        material["A1"] = p.parseMathematicaString(Data::XRite_Reflectance_A1).toVisibleFull();
        material["E2"] = p.parseMathematicaString(Data::XRite_Reflectance_E2).toVisibleFull();
        material["F4"] = p.parseMathematicaString(Data::XRite_Reflectance_F4).toVisibleFull();
        material["G4"] = p.parseMathematicaString(Data::XRite_Reflectance_G4).toVisibleFull();
        material["H4"] = p.parseMathematicaString(Data::XRite_Reflectance_H4).toVisibleFull();
        material["J4"] = p.parseMathematicaString(Data::XRite_Reflectance_J4).toVisibleFull();
    }
};

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

int main(int argc, char **argv)
{
    InputParser input(argc, argv);
    if (input.cmdOptionExists("-h") || input.cmdOptionExists("--help"))
    {
        std::cout << "spectrum [-n RANDOM_SAMPLE_COUNT] [-m EQUIDISTANT_SAMPLE_COUNT]\n";
        std::cout << "spectrum --demo\n";
        return EXIT_SUCCESS;
    }

    SpectralMultiplication sm;
    if (argc == 2 && input.cmdOptionExists("--demo"))
    {
        sm.runDemo();
        return EXIT_SUCCESS;
    }

    RunParams params;
    if (auto o = input.getCmdOption("-n"); !o.empty())
        params.randomSampleCount = std::stoi(o);
    if (auto o = input.getCmdOption("-m"); !o.empty())
        params.equidistantSampleCount = std::stoi(o);

    sm.run(params);

    return EXIT_SUCCESS;
}
