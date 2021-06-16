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
        std::cout.setf(std::ios::fixed);
        std::cout.precision(2);

        Result::REFERENCE.printT("REFERENCE");
        std::cout << "\n";
    }

    void run(const RunParams& params) const
    {
        Sampler::Uniform uSampler;
        Sampler::Hero hSampler;
        Result uRes, hRes, eRes;

        for (const auto& [lumName, lumSpectrum] : luminaries)
            for (const auto& [matName, matSpectrum] : materials)
            {
                uRes.values[lumName][matName] = ColorSpace::RGB(uSampler.eval(params.randomSampleCount, lumSpectrum, matSpectrum));
                hRes.values[lumName][matName] = ColorSpace::RGB(hSampler.eval(params.randomSampleCount, lumSpectrum, matSpectrum));
                eRes.values[lumName][matName] = ColorSpace::RGB(Sampler::Equidistant::eval(params.equidistantSampleCount, lumSpectrum, matSpectrum));
            }
        uRes.evalPrint("Random uniform sampling (" + std::to_string(params.randomSampleCount) + ")");
        hRes.evalPrint("Hero wavelength sampling (" + std::to_string(params.randomSampleCount / 4) + ")");
        eRes.evalPrint("Equidistant sampling (" + std::to_string(params.equidistantSampleCount) + ")");
    }

    void runDemo() const
    {
        Result full, uniform, hero, equidistant;
        for (const auto& [lumName, lumSpectrum] : luminaries)
            for (const auto& [matName, matSpectrum] : materials)
                full.values[lumName][matName] = ColorSpace::RGB(lumSpectrum * matSpectrum);
        full.evalPrint("Full spectral evaluation");

        run(RunParams{75, 8});
        run(RunParams{125, 25});
        run(RunParams{200, 45});
    }

    void printSpectralData() const
    {
        for(const auto& [name, spectrum] : luminaries)
        {
            std::cout << "\n" << name;
            spectrum.print();
        }
        for(const auto& [name, spectrum] : materials)
        {
            std::cout << "\n" << name;
            spectrum.print();
        }
    }

private:
    std::unordered_map<std::string, Spectrum::VisibleFull> luminaries;
    std::unordered_map<std::string, Spectrum::VisibleFull> materials;

    void loadSpectralData()
    {
        Spectrum::Parser p;
        // up-sampling using linear interpolation
        luminaries[" A "] = p.parseMathematicaString(Data::CIE_Illuminant_A).toVisibleFull();
        luminaries["D65"] = p.parseMathematicaString(Data::CIE_Illuminant_D65).toVisibleFull();
        luminaries["F11"] = p.parseMathematicaString(Data::CIE_Illuminant_F11).toVisibleFull();
        materials["A1"] = p.parseMathematicaString(Data::XRite_Reflectance_A1).toVisibleFull();
        materials["E2"] = p.parseMathematicaString(Data::XRite_Reflectance_E2).toVisibleFull();
        materials["F4"] = p.parseMathematicaString(Data::XRite_Reflectance_F4).toVisibleFull();
        materials["G4"] = p.parseMathematicaString(Data::XRite_Reflectance_G4).toVisibleFull();
        materials["H4"] = p.parseMathematicaString(Data::XRite_Reflectance_H4).toVisibleFull();
        materials["J4"] = p.parseMathematicaString(Data::XRite_Reflectance_J4).toVisibleFull();
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
        if (auto itr =  std::find(tokens.cbegin(), tokens.cend(), option);itr != tokens.cend() && ++itr != tokens.end())
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
    const InputParser input(argc, argv);
    if (argc == 1 || input.cmdOptionExists("-h") || input.cmdOptionExists("--help"))
    {
        std::cout << "spectrum [-n RANDOM_SAMPLE_COUNT] [-m EQUIDISTANT_SAMPLE_COUNT]\n";
        std::cout << "spectrum --demo\n";
        return EXIT_SUCCESS;
    }

    const SpectralMultiplication sm;
    if (argc == 2 && input.cmdOptionExists("--demo"))
    {
        sm.runDemo();
        return EXIT_SUCCESS;
    }

    RunParams params;
    if (const auto o = input.getCmdOption("-n"); !o.empty())
        params.randomSampleCount = std::stoi(o);
    if (const auto o = input.getCmdOption("-m"); !o.empty())
        params.equidistantSampleCount = std::stoi(o);

    sm.run(params);

    return EXIT_SUCCESS;
}
