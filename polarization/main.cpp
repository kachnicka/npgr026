#include <iostream>
#include <vector>

#include "Polarization.h"

namespace Scene
{
    using Light = glm::vec4;
    using StokesVector = glm::vec4;

    struct Interface
    {
        float theta = 0.0f;
        float eta = 1.0f;
        float etaK = 0.0f;
    };

    struct Result
    {
        short id = 0;
        StokesVector sv = {};

        static void printHeader()
        {
            std::cout << "Test runs: \n";
            std::cout << "id\ts0\ts1\ts2\ts3\n";
        }
        void print() const
        {
            std::cout << id << '\t' << sv.x << '\t' << sv.y << '\t' << sv.z << '\t' << sv.w << '\n';
        }
    };

    class Scene
    {
        static Light unpolarized;
        short id = 0;
        Interface x1, x2;
        float rho = 0.0f;
        float filterRot = 0.0f;
        std::unique_ptr<glm::mat4> filter;

    public:
        Result traverse() const
        {
            Result r{id, {}};

            r.sv = unpolarized;

            return r;
        }

        static void printHeader()
        {
            std::cout << "Light: " << glm::to_string(unpolarized) << "\n";
            std::cout << "id\tn1\tk1\tn2\tk2\tdelta\trho\tphi\tfilter\n";
        }
        void print() const
        {
            std::cout << id << '\t' << x1.eta << '\t' << x1.etaK << '\t' << x2.eta << '\t' << x2.etaK << '\t' << x1.theta << '\t' << rho << '\t' << x2.theta << '\t';
            if (filter)
                std::cout << filterRot << "\n";
            else
                std::cout << "---" << "\n";
        }

        Scene(const short id, const Interface x1, const Interface x2, const bool insertFilter = false, const float filterRot = 0.0f) :
            id(id), x1(x1), x2(x2), filterRot(filterRot)
        {
            if (insertFilter)
                filter = std::make_unique<glm::mat4>(MuellerMatrix::LinearFilter(filterRot * DEG_TO_RAD));
        }
    };
    Light Scene::unpolarized = { 100.0f, 0.0f, 0.0f, 0.0f };

}

int main(int argc, char **argv)
{
    std::cout.setf(std::ios::fixed);
    std::cout.precision(2);

    std::vector<Scene::Scene> testScenes;
    testScenes.reserve(9);
    testScenes.emplace_back(Scene::Scene{ 1, { 53.0f, 1.33f, 0.0f }, { 56.0f, 1.5f, 0.0f } });
    testScenes.emplace_back(Scene::Scene{ 2, { 53.0f, 1.33f, 0.0f }, { 56.0f, 1.5f, 0.0f } , true });
    testScenes.emplace_back(Scene::Scene{ 3, { 53.0f, 1.33f, 0.0f }, { 56.0f, 1.5f, 0.0f } , true, 90.0f });

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

    //Ray unpolarized;
    //unpolarized.sv = { 100.0f, 0.0f, 0.0f, 0.0f };
    //Ray horizontallyPolarized;
    //horizontallyPolarized.sv = { 100.0f, 100.0f, 0.0f, 0.0f };
    //Ray verticallyPolarized;
    //verticallyPolarized.sv = { 100.0f, -100.0f, 0.0f, 0.0f };

    //const Interaction noAttenuation(unpolarized, MuellerMatrix::PlainAttenuation(1));
    //const Interaction halfAttenuation(unpolarized, MuellerMatrix::PlainAttenuation(0.5));

    //{
    //    const Interaction filtered0(unpolarized, MuellerMatrix::LinearFilter(0));
    //    const Interaction filtered1(unpolarized, MuellerMatrix::LinearFilter(0.5f * M_PI));
    //}

    //{
    //    const Interaction filtered0(horizontallyPolarized, MuellerMatrix::LinearFilter(0));
    //    const Interaction filtered1(horizontallyPolarized, MuellerMatrix::LinearFilter(0.5f * M_PI));
    //}


    return EXIT_SUCCESS;
}
