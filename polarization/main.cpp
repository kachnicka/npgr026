#include <iostream>
#include <vector>

#include "Polarization.h"

namespace Scene
{
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
        static StokesVector unpolarizedLight;
        short id = 0;
        Interface x1, x2;
        float rho = 0.0f;
        float filterRot = 0.0f;
        std::unique_ptr<glm::mat4> filter;

    public:
        [[nodiscard]] Result traverse() const
        {
            Result r{id, {}};

            const auto mmX1 = MuellerMatrix::FresnelReflectance(FresnelGeneral(cos(x1.theta * DEG_TO_RAD), x1.eta, x1.etaK));
            auto mmX2 = MuellerMatrix::FresnelReflectance(FresnelGeneral(cos(x2.theta * DEG_TO_RAD), x2.eta, x2.etaK));
            auto mmF = filter ? *filter : glm::mat4(1.0f);

            if (rho > 0.0f)
            {
                mmX2 = MuellerMatrix::Rotate(mmX2, rho * DEG_TO_RAD);
                mmF = MuellerMatrix::Rotate(mmF, rho * DEG_TO_RAD);
            }

            // matrix multiplication in glm is done from right to left
            const auto mm = mmX1 * mmX2 * mmF;
            r.sv =  mm * unpolarizedLight;

            return r;
        }

        static void printHeader()
        {
            std::cout << "Light: " << glm::to_string(unpolarizedLight) << "\n";
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

        Scene(const short id, const Interface x1, const Interface x2, const bool insertFilter = false, const float filterRot = 0.0f, const float rho = 0.0f) :
            id(id), x1(x1), x2(x2), rho(rho), filterRot(filterRot)
        {
            if (insertFilter)
                filter = std::make_unique<glm::mat4>(MuellerMatrix::LinearFilter(filterRot * DEG_TO_RAD));
        }
    };
    StokesVector Scene::unpolarizedLight = { 100.0f, 0.0f, 0.0f, 0.0f };

}

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
