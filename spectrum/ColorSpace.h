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
        static const Spectrum::VisibleFull XCurve;
        static const Spectrum::VisibleFull YCurve;
        static const Spectrum::VisibleFull ZCurve;
    public:
        glm::vec3 color;
        explicit XYZ(const Spectrum::VisibleFull& spectrum) : color((XCurve * spectrum).sum(), (YCurve * spectrum).sum(), (ZCurve * spectrum).sum()) {}
    };

    class RGB
    {
        static const glm::tmat3x3<float> XYZtoRGBTransform;
    public:
        glm::vec3 color;
        explicit RGB(const XYZ& color) : color(toRGB(color)) {}
        explicit RGB(const Spectrum::VisibleFull& spectrum) : RGB(XYZ(spectrum)) {}
    private:
        static glm::vec3 toRGB(const XYZ& color)
        {
            return XYZtoRGBTransform * color.color;
        }
    };

    const Spectrum::VisibleFull XYZ::XCurve = Spectrum::Parser().parseMathematicaString(Data::CIE_X).toVisibleFull();
    const Spectrum::VisibleFull XYZ::YCurve = Spectrum::Parser().parseMathematicaString(Data::CIE_Y).toVisibleFull();
    const Spectrum::VisibleFull XYZ::ZCurve = Spectrum::Parser().parseMathematicaString(Data::CIE_Z).toVisibleFull();

    const glm::tmat3x3<float> RGB::XYZtoRGBTransform =
        {
            3.2410, -0.9692, 0.0556,
            -1.5374, 1.8760, -0.2040,
            -0.4986, 0.0416, 1.0570,
        };
}
