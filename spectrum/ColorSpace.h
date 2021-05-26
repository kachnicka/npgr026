#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "Spectrum.h"
#include "SpectralData.h"

namespace ColorSpace
{
    class RGB;
    class XYZ
    {
        static const Spectrum::VisibleFull X_CURVE;
        static const Spectrum::VisibleFull Y_CURVE;
        static const Spectrum::VisibleFull Z_CURVE;
    public:
        glm::vec3 color;
        explicit XYZ(const Spectrum::VisibleFull& spectrum) : color((X_CURVE * spectrum).sum(), (Y_CURVE * spectrum).sum(), (Z_CURVE * spectrum).sum()) {}
    };

    class RGB
    {
        static const glm::tmat3x3<float> XYZ_TO_RGB_MATRIX;
    public:
        glm::vec3 color{};
        RGB() = default;
        explicit RGB(const glm::vec3& color) : color(color) {}
        explicit RGB(const XYZ& color) : color(toRGB(color)) {}
        explicit RGB(const Spectrum::VisibleFull& spectrum) : RGB(XYZ(spectrum)) {}

        RGB& operator/=(const RGB& rhs)
        {
            color /= rhs.color;
            return *this;
        }

        friend RGB operator/(RGB lhs, const RGB& rhs)
        {
            return lhs /= rhs;
        }

        RGB& operator-=(const RGB& rhs)
        {
            color -= rhs.color;
            return *this;
        }

        friend RGB operator-(RGB lhs, const RGB& rhs)
        {
            return lhs -= rhs;
        }
    private:
        static glm::vec3 toRGB(const XYZ& color)
        {
            return XYZ_TO_RGB_MATRIX * color.color;
        }
    };

    const Spectrum::VisibleFull XYZ::X_CURVE = Spectrum::Parser().parseMathematicaString(Data::CIE_X).toVisibleFull();
    const Spectrum::VisibleFull XYZ::Y_CURVE = Spectrum::Parser().parseMathematicaString(Data::CIE_Y).toVisibleFull();
    const Spectrum::VisibleFull XYZ::Z_CURVE = Spectrum::Parser().parseMathematicaString(Data::CIE_Z).toVisibleFull();

    const glm::tmat3x3<float> RGB::XYZ_TO_RGB_MATRIX =
        {
            3.2410, -0.9692, 0.0556,
            -1.5374, 1.8760, -0.2040,
            -0.4986, 0.0416, 1.0570,
        };
}
