#ifndef slic3r_ExPolygon_hpp_
#define slic3r_ExPolygon_hpp_

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
}

#include "Point.hpp"

typedef std::vector<Point> Polygon;
typedef std::vector<Polygon> Polygons;

class ExPolygon
{
    public:
    Polygon contour;
    Polygons holes;
    SV* arrayref();
    void scale(double factor);
};

#define scale_polygon(poly, factor) \
    for (Polygon::iterator pit = (poly).begin(); pit != (poly).end(); ++pit) { \
        (*pit).x *= factor; \
        (*pit).y *= factor; \
    }

void
ExPolygon::scale(double factor)
{
    scale_polygon(contour, factor);
    for (Polygons::iterator it = holes.begin(); it != holes.end(); ++it) {
        scale_polygon(*it, factor);
    }
}

void
perl2polygon(SV* poly_sv, Polygon& poly)
{
    AV* poly_av = (AV*)SvRV(poly_sv);
    const unsigned int num_points = av_len(poly_av)+1;
    poly.resize(num_points);
    
    for (unsigned int i = 0; i < num_points; i++) {
        SV** point_sv = av_fetch(poly_av, i, 0);
        AV*  point_av = (AV*)SvRV(*point_sv);
        Point& p = poly[i];
        p.x = (unsigned long)SvIV(*av_fetch(point_av, 0, 0));
        p.y = (unsigned long)SvIV(*av_fetch(point_av, 1, 0));
    }
}

SV*
polygon2perl(Polygon& poly) {
    const unsigned int num_points = poly.size();
    AV* av = newAV();
    av_extend(av, num_points-1);
    for (unsigned int i = 0; i < num_points; i++) {
        av_store(av, i, point2perl(poly[i]));
    }
    return sv_bless(newRV_noinc((SV*)av), gv_stashpv("Slic3r::Polygon", GV_ADD));
}

#endif