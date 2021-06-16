#pragma once
#include <cmath>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/string_cast.hpp>

constexpr float M_PI = 3.14159265358979323846f;
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

		const auto aSqr = (t1 + t0) / 2;
		const auto bSqr = (t1 - t0) / 2;
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
		delta_s = atan(2 * cosTheta / (cosThetaSqr - t1));
		delta_p = atan(2 * b * cosTheta * ((etaSqr - etaKSqr) * b - 2 * eta * etaK * a) / ((etaSqr + etaKSqr) * (etaSqr + etaKSqr) * cosThetaSqr - t1));
	}
};

struct Ray
{
	glm::vec4 sv = {};
};

class StokesVector
{
	glm::vec4 sv = {};
};

struct MMAttenuationInterface
{
	glm::mat4 mm = {};

    [[nodiscard]] Ray interact(const Ray& iRay) const
	{
		Ray oRay;
		oRay.sv = mm * iRay.sv;
		return oRay;
	}
};

struct Interaction
{
	Ray iRay;
	MMAttenuationInterface i;
	Ray oRay;
	Interaction(Ray iRay, glm::mat4 mm) : iRay(iRay), i({ mm }), oRay({ i.mm * iRay.sv})
	{
		print();

		auto ke0 = i.mm * iRay.sv;
		auto ke1 = iRay.sv * i.mm;
	}
	void print() const
	{
		std::cout << "Interaction:" << ":\n";
		std::cout << "iRay:\n";
		std::cout << glm::to_string(iRay.sv) << ">\n";
		std::cout << "Mueller matrix:\n";
		std::cout << glm::to_string(i.mm) << ">\n";
		std::cout << "oRay:\n"; 
		std::cout << glm::to_string(oRay.sv) << ">\n\n";
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

		return glm::mat4(1.0f, cosA, sinA, 0.0f,
			cosA, cosA * cosA, sinA * cosA, 0.0f,
			sinA, sinA * cosA, sinA * sinA, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f) * 0.5f;
	}

};

