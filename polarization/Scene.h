#pragma once

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

            return { id, mm * unpolarizedLight };
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

