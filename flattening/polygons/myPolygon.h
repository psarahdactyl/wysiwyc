#pragma once
#ifndef MYPOLYGON_H
#define MYPOLYGON_H

#include <string>
#include <vector>

// CGAL includes and typedefs
#include <CGAL/Cartesian.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Arr_Bezier_curve_traits_2.h>
#include <CGAL/Gps_traits_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Timer.h>

typedef CGAL::CORE_algebraic_number_traits              Nt_traits;
typedef Nt_traits::Rational                             Rational;
typedef Nt_traits::Algebraic                            Algebraic;
typedef CGAL::Cartesian<Rational>                       Rat_kernel;
typedef CGAL::Cartesian<Algebraic>                      Alg_kernel;
typedef CGAL::Arr_Bezier_curve_traits_2<Rat_kernel, Alg_kernel, Nt_traits>
Traits_2;

typedef Rat_kernel::Point_2                             Rat_point_2;
typedef Alg_kernel::Point_2								Alg_point_2;
typedef Traits_2::Curve_2                               Bezier_curve_2;
typedef Traits_2::X_monotone_curve_2                    X_monotone_curve_2;
typedef CGAL::Gps_traits_2<Traits_2>                    Gps_traits_2;
typedef Gps_traits_2::General_polygon_2                 Polygon_2;
typedef Gps_traits_2::General_polygon_with_holes_2      Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                 Polygon_set;
typedef CGAL::General_polygon_set_2<Gps_traits_2>		Bezier_polygon_set;

class myPolygon
{
public:
	myPolygon();
	~myPolygon();
	myPolygon(bool closed, int fill, int stroke, Polygon_with_holes_2 cgalPoly);
	Polygon_with_holes_2 cgalPolygon;
	bool closed;
	int fill;
	int stroke;
};

#endif
