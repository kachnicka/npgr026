#pragma once
#include <cmath>
#include <glm/mat4x4.hpp>
#include <glm/gtx/string_cast.hpp>

#ifndef M_PI
constexpr float M_PI = 3.14159265358979323846f;
#endif
constexpr float DEG_TO_RAD = 0.017453293f;
constexpr float RAD_TO_DEG = 57.29577951f;

class FresnelGeneral
{
public:
    // reflectance
    float r_s;
    float r_p;
    // retardance
    float delta_s;
    float delta_p;

    FresnelGeneral(float cosTheta, float eta, float etaK)
    {
        const auto cosThetaSqr = cosTheta * cosTheta;
        const auto sinThetaSqr = 1.0f - cosThetaSqr;
        const auto etaSqr = eta * eta;
        const auto etaKSqr = etaK * etaK;

        const auto t0 = etaSqr - etaKSqr - sinThetaSqr;
        const auto t1 = sqrt(t0 * t0 + 4 * etaSqr * etaKSqr);

        const auto aSqr = std::max(0.0, (t1 + t0) * 0.5f);
        const auto bSqr = std::max(0.0, (t1 - t0) * 0.5f);
        const auto a = sqrt(aSqr);
        const auto b = sqrt(bSqr);

        // aSqr + bSqr = t1
        // aSqr - bSqr = t0
        const auto t2 = t1 + cosThetaSqr;
        const auto t3 = 2 * a * cosTheta;

        const auto t4 = cosThetaSqr * t1 + sinThetaSqr * sinThetaSqr;
        const auto t5 = t3 * sinThetaSqr;

        r_s = (t2 - t3) / (t2 + t3);
        r_p = (t4 - t5) / (t4 + t5) * r_s;
        delta_s = atan2(2 * b * cosTheta , (cosThetaSqr - t1));
        delta_p = atan2(2  * cosTheta * ((etaSqr - etaKSqr) * b - 2 * eta * etaK * a) , ((etaSqr + etaKSqr) * (etaSqr + etaKSqr) * cosThetaSqr - t1));
    }
};

namespace MuellerMatrix
{
    static glm::mat4 PlainAttenuation(float attenuationFactor = 1.0f)
    {
        glm::mat4 m = {};
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = attenuationFactor;
        return m;
    }

    static glm::mat4 Depolarizer(float attenuationFactor = 1.0f)
    {
        glm::mat4 m = {};
        m[0][0] = attenuationFactor;
        return m;
    }

    static glm::mat4 LinearFilter(const float phi = 0.0f)
    {
        const auto a = 2 * phi;
        const auto cosA = cos(a);
        const auto sinA = sin(a);

        return 0.5f * glm::mat4(
            1.0f, cosA, sinA, 0.0f,
            cosA, cosA * cosA, sinA * cosA, 0.0f,
            sinA, sinA * cosA, sinA * sinA, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f);
    }

    static glm::mat4 FresnelReflectance(const FresnelGeneral& f)
    {
        const auto A = (f.r_s + f.r_p) * 0.5f;
        const auto B = (f.r_s - f.r_p) * 0.5f;

        const auto delta = f.delta_s - f.delta_p;
        const auto t = sqrt(f.r_s * f.r_p);

        const auto C = cos(delta) * t;
        const auto S = sin(delta) * t;

        return glm::mat4(
            A, B, 0.0f, 0.0f,
            B, A, 0.0f, 0.0f,
            0.0f, 0.0f, C, S,
            0.0f, 0.0f, -S, C);
    }

    // optimized rotation (simplified matrix multiplication)
    // based on https://nvlpubs.nist.gov/nistpubs/Legacy/TN/nbstechnicalnote910-3.pdf equation 6.39 (page 37)
    static glm::mat4 Rotate(const glm::mat4& mm, float phi)
    {
        const auto S = sin(2 * phi);
        const auto C = cos(2 * phi);

        const auto A = (mm[1][1] - mm[2][2]) * S * S + (mm[2][1] + mm[1][2]) * S * C;
        const auto B = (mm[1][1] - mm[2][2]) * S * C + (mm[2][1] + mm[1][2]) * S * S;

        return glm::mat4(
            mm[0][0],                    mm[1][0] * C - mm[2][0] * S, mm[1][0] * S + mm[2][0] * C, mm[3][0],
            mm[0][1] * C - mm[0][2] * S, mm[1][1] - A,                mm[2][1] + B,                mm[3][1] * C - mm[3][2] * S,
            mm[0][1] * S + mm[0][2] * C, mm[1][2] + B,                mm[2][2] + A,                mm[3][1] * S + mm[3][2] * C,
            mm[0][3],                    mm[1][3] * C - mm[2][3] * S, mm[1][3] * S + mm[2][3] * C, mm[3][3]);
    }
};

