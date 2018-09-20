#pragma once

// regular STL includes
#include <algorithm>
#include <vector>
#include <string>

// c and nanoSVG includes
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nanosvg.h"

// boost includes
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>

// CGAL includes
#include <CGAL/Cartesian.h>
#include <CGAL/iterator.h>
#include <CGAL/bounding_box.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/CORE_BigRat.h>
#include <CGAL/Epeck_d.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>

// Eigen includes
//#include <Eigen/Dense>
//#include <Eigen/Sparse>

// tinyxml includes
#include "tinyxml2.h"

// CGAL typedefs
typedef CGAL::CORE_algebraic_number_traits					Nt_traits;
typedef Nt_traits::Rational									Rational;
typedef Nt_traits::Algebraic								Algebraic;
typedef CGAL::Cartesian<Rational>							Rat_kernel;
typedef CGAL::Cartesian<Algebraic>							Alg_kernel;
typedef CGAL::Exact_predicates_exact_constructions_kernel	K;

// CGAL classes
typedef K::Point_2											Point;
typedef K::Segment_2										Segment;
typedef K::Ray_2											Ray;
typedef K::Line_2											Line;
typedef K::Iso_rectangle_2									Rectangle;
typedef CGAL::Polygon_2<K>									Polygon;

// nanosvg object
typedef NSVGshape*											Shape;

// my typedefs
typedef std::vector<Shape>									Shape_set;
typedef std::vector<int>									Shape_indices;

typedef std::vector<Point>									Point_set;
typedef std::vector<Segment>								Segment_set;
typedef std::vector<std::vector<Segment> >					Shape_segment_set;

typedef std::pair<Point, Segment>							Intersection; // (intersection point, segment which causes the intersection)
typedef std::pair<Segment, Segment_set>						Split_intersection; // (segment piece, set of segments which cause the segment)
typedef std::vector<Intersection>							Intersection_set;
typedef std::vector<Split_intersection>						Split_intersection_set;

struct Segment_w_intersection
{
	Segment s;
	Point intersection;
};


template<typename O>
bool already_in_set(std::vector<O>& set, O& object)
{
	if (std::find(set.begin(), set.end(), object) != set.end()))
	{
		return true;
	}
	return false;
}