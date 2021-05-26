#pragma once

#include "Spectrum.h"
#include "SpectralData.h"

namespace ColorSpace
{
    class XYZ
    {
        static Spectrum::VisibleFull XCurve;
        static Spectrum::VisibleFull YCurve;
        static Spectrum::VisibleFull ZCurve;
    public:
        explicit XYZ(const Spectrum::VisibleFull& spectrum) : x((XCurve * spectrum).sum()), y((YCurve * spectrum).sum()), z((ZCurve * spectrum).sum())
        {}
        float x, y, z;
    };

    Spectrum::VisibleFull XYZ::XCurve = Spectrum::Parser().parseMathematicaString(Data::CIE_X).toVisibleFull();
    Spectrum::VisibleFull XYZ::YCurve = Spectrum::Parser().parseMathematicaString(Data::CIE_X).toVisibleFull();
    Spectrum::VisibleFull XYZ::ZCurve = Spectrum::Parser().parseMathematicaString(Data::CIE_X).toVisibleFull();
}
