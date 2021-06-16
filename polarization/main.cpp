#include <iostream>
#include <vector>

#include "Polarization.h"
#include "Scene.h"

int main(int argc, char **argv)
{
    std::cout.setf(std::ios::fixed);
    std::cout.precision(3);
    const auto etaInGlass = 1.0f / 1.5105f;

    std::vector<Scene::Scene> testScenes;
    testScenes.reserve(9);
    testScenes.emplace_back(Scene::Scene{ 1, { 53.0f, 1.33f, 0.0f }, { 56.0f, 1.5f, 0.0f } });
    testScenes.emplace_back(Scene::Scene{ 2, { 53.0f, 1.33f, 0.0f }, { 56.0f, 1.5f, 0.0f } , true });
    testScenes.emplace_back(Scene::Scene{ 3, { 53.0f, 1.33f, 0.0f }, { 56.0f, 1.5f, 0.0f } , true, 90.0f });
    testScenes.emplace_back(Scene::Scene{ 4, { 48.0f, etaInGlass, 0.0f }, { 54.6f, etaInGlass, 0.0f } , true, 0.0f });
    testScenes.emplace_back(Scene::Scene{ 5, { 48.0f, etaInGlass, 0.0f }, { 54.6f, etaInGlass, 0.0f } , true, 90.0f });
    testScenes.emplace_back(Scene::Scene{ 6, { 48.0f, etaInGlass, 0.0f }, { 54.6f, etaInGlass, 0.0f } , true, 45.0f });
    testScenes.emplace_back(Scene::Scene{ 7, { 48.0f, 1.12f, 2.16f }, { 20.0f, 0.608f, 2.12f } , false, 0.0f, 34.0f });
    testScenes.emplace_back(Scene::Scene{ 8, { 48.0f, 1.12f, 2.16f }, { 20.0f, 0.608f, 2.12f } , true, 0.0f, 34.0f });
    testScenes.emplace_back(Scene::Scene{ 9, { 48.0f, 1.12f, 2.16f }, { 20.0f, 0.608f, 2.12f } , true, 90.0f, 34.0f });

    std::vector<Scene::Result> results;
    results.reserve(testScenes.size());

    for (const auto& s : testScenes)
        results.emplace_back(s.traverse());

    Scene::Scene::printHeader();
    for (const auto& s : testScenes)
        s.print();

    std::cout << '\n';
    Scene::Result::printHeader();
    for (const auto& r : results)
        r.print();

    return EXIT_SUCCESS;
}
