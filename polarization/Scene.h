#pragma once
#include <memory>

namespace Scene
{
    using StokesVec = glm::vec4;
    using MuellerMat = glm::mat4;

    struct FresnelSurface
    {
        float theta = 0.0f;
        float eta = 1.0f;
        float etaK = 0.0f;

        [[nodiscard]] MuellerMat getMuellerMatrix() const
        {
            return MuellerMatrix::FresnelReflectance(FresnelGeneral(cos(theta * DEG_TO_RAD), eta, etaK));
        }
    };

    struct LinearFilter
    {
        float filterRot = 0.0f;

        [[nodiscard]] MuellerMat getMuellerMatrix() const
        {
            return MuellerMatrix::LinearFilter(filterRot * DEG_TO_RAD);
        }
    };

    struct Result
    {
        short id = 0;
        StokesVec sv = {};

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

    class RayState
    {
        MuellerMat m{1.0f};
        float frameRotation = 0.0f;
    public:
        void addInterfaceInteraction(MuellerMat mm, float interfaceRot = 0.0f)
        {
            frameRotation += interfaceRot * DEG_TO_RAD;
            if (frameRotation != 0.0f)
                mm = MuellerMatrix::Rotate(mm, frameRotation);
            m = m * mm;
        }

        [[nodiscard]] StokesVec lightInteraction(const StokesVec& light) const
        {
            return m * light;
        }
    };

    class Scene
    {
        static StokesVec unpolarizedLight;

        short id = 0;
        FresnelSurface x1, x2;
        float rho = 0.0f;
        std::unique_ptr<LinearFilter> filter;

    public:
        [[nodiscard]] Result traverse() const
        {
            RayState compoundMM;

            // emulation of light path from camera to source
            compoundMM.addInterfaceInteraction(x1.getMuellerMatrix());
            compoundMM.addInterfaceInteraction(x2.getMuellerMatrix(), rho);
            if (filter)
                compoundMM.addInterfaceInteraction(filter->getMuellerMatrix());

            return { id, compoundMM.lightInteraction(unpolarizedLight) };
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
                std::cout << filter->filterRot << "\n";
            else
                std::cout << "---" << "\n";
        }

        Scene(const short id, const FresnelSurface x1, const FresnelSurface x2, const bool insertFilter = false, const float filterRot = 0.0f, const float rho = 0.0f) :
            id(id), x1(x1), x2(x2), rho(rho)
        {
            if (insertFilter)
                filter = std::make_unique<LinearFilter>(LinearFilter{filterRot});
        }
    };
    StokesVec Scene::unpolarizedLight = { 100.0f, 0.0f, 0.0f, 0.0f };
}

