#pragma once

#include <unordered_map>
#include <glm/vec3.hpp>

#include "ColorSpace.h"

class Result
{
public:
    static const Result REFERENCE;
    static const std::array<std::string, 3> lumSorted;
    static const std::array<std::string, 6> matSorted;
    std::unordered_map<std::string, std::unordered_map<std::string, ColorSpace::RGB>> values;

    void evalPrint(const std::string& header) const
    {
        printT(header);
        getAbsDiff().printT("ABS DIFF");
        std::cout << "\n";
    }

    static Result getReference()
    {
        Result r;
        r.values[" A "]["A1"] = ColorSpace::RGB(glm::vec3(83.35, 37.84, 10.54));
        r.values[" A "]["E2"] = ColorSpace::RGB(glm::vec3(11.99, 2.01, 0.032));
        r.values[" A "]["F4"] = ColorSpace::RGB(glm::vec3(6.33, 11.02, -0.18));
        r.values[" A "]["G4"] = ColorSpace::RGB(glm::vec3(34.51, -1.38, -0.22));
        r.values[" A "]["H4"] = ColorSpace::RGB(glm::vec3(69.19, 22.74, -2.91));
        r.values[" A "]["J4"] = ColorSpace::RGB(glm::vec3(1.12, 8.60, 5.43));
        r.values["D65"]["A1"] = ColorSpace::RGB(glm::vec3(80.79, 82.44, 80.84));
        r.values["D65"]["E2"] = ColorSpace::RGB(glm::vec3(13.03, 4.42, 2.02));
        r.values["D65"]["F4"] = ColorSpace::RGB(glm::vec3(2.76, 24.67, 2.61));
        r.values["D65"]["G4"] = ColorSpace::RGB(glm::vec3(35.52, -0.60, 1.09));
        r.values["D65"]["H4"] = ColorSpace::RGB(glm::vec3(73.73, 48.95, -2.29));
        r.values["D65"]["J4"] = ColorSpace::RGB(glm::vec3(-5.53, 20.42, 32.49));
        r.values["F11"]["A1"] = ColorSpace::RGB(glm::vec3(1867.8, 1243.61, 704.05));
        r.values["F11"]["E2"] = ColorSpace::RGB(glm::vec3(274.45, 66.78, 13.33));
        r.values["F11"]["F4"] = ColorSpace::RGB(glm::vec3(118.61, 378.68, -0.67));
        r.values["F11"]["G4"] = ColorSpace::RGB(glm::vec3(680.00, -3.56, 3.49));
        r.values["F11"]["H4"] = ColorSpace::RGB(glm::vec3(1520.02, 812.03, -87.00));
        r.values["F11"]["J4"] = ColorSpace::RGB(glm::vec3(30.39, 260.78, 290.40));
        return r;
    }

    void print(const std::string& header) const
    {
        std::cout << header << ":\n";
        for (const auto& lumName : lumSorted)
        {
            std::cout << "<" << lumName << ">\t";
            for (const auto& matName : matSorted)
            {
                const auto rgb = values.at(lumName).at(matName);
                std::cout << "<" << matName << ">\t" << rgb.color.r << "\t" << rgb.color.g << "\t" << rgb.color.b << "\t";
            }
            std::cout << "\n";
        }
    }

    void printT(const std::string& header) const
    {
        std::cout << header << ":\n";
        for (const auto& matName : matSorted)
        {
            std::cout << "<" << matName << ">\t";
            for (const auto& lumName : lumSorted)
            {
                const auto rgb = values.at(lumName).at(matName);
                std::cout << "<" << lumName << ">\t" << rgb.color.r << "\t" << rgb.color.g << "\t" << rgb.color.b << "\t";
            }
            std::cout << "\n";
        }
    }


private:
    Result getAbsDiff() const
    {
        Result diff;
        for (const auto& lumName : lumSorted)
            for (const auto& matName : matSorted)
            {
                auto& value = diff.values[lumName][matName];
                value = values.at(lumName).at(matName) - REFERENCE.values.at(lumName).at(matName);
                value.color = glm::abs(value.color);
            }
        return diff;
    }
};

const Result Result::REFERENCE = Result::getReference();
const std::array<std::string, 3> Result::lumSorted = { " A ", "D65", "F11"};
const std::array<std::string, 6> Result::matSorted = { "A1", "E2", "F4", "G4", "H4", "J4"};
