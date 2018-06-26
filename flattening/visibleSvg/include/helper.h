#pragma once

// regular STL includes
#include <algorithm>
#include <vector>

// c and nanoSVG includes
#include <stdio.h>
#include <string.h>
#include <math.h>
//#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

// CGAL includes
#include <CGAL/Cartesian.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/CORE_BigRat.h>
#include <CGAL/Arr_Bezier_curve_traits_2.h>
#include <CGAL/Arrangement_with_history_2.h>
#include <CGAL/Surface_sweep_2_algorithms.h>
#include <CGAL/Gps_traits_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Timer.h>
#include <CGAL/Arr_face_index_map.h>
#include <CGAL/graph_traits_Dual_Arrangement_2.h>
#include <CGAL/boost/graph/Dual.h>
#include <CGAL/Arr_observer.h>
#include <CGAL/Arr_extended_dcel.h>

// boost includes
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>

// Eigen includes
#include <Eigen/Dense>
#include <Eigen/Sparse>

// CGAL typedefs
typedef CGAL::CORE_algebraic_number_traits					Nt_traits;
typedef Nt_traits::Rational									Rational;
typedef Nt_traits::Algebraic								Algebraic;
typedef CGAL::Cartesian<Rational>							Rat_kernel;
typedef CGAL::Cartesian<Algebraic>							Alg_kernel;
typedef CGAL::Arr_Bezier_curve_traits_2<Rat_kernel, Alg_kernel, Nt_traits>
Traits_2;

typedef Rat_kernel::Point_2									Rat_point_2;
typedef Alg_kernel::Point_2									Alg_point_2;
typedef Traits_2::Curve_2									Bezier_curve_2;
typedef Traits_2::X_monotone_curve_2						X_monotone_curve_2;

typedef CGAL::Arr_face_extended_dcel<Traits_2, int>			Dcel;
typedef CGAL::Arrangement_with_history_2<Traits_2, Dcel>	Arrangement_2;
typedef CGAL::Dual<Arrangement_2>							Dual_arrangement;

typedef Traits_2::Point_2									Point;
typedef CGAL::Arr_face_index_map<Arrangement_2>				Face_index_map;
typedef Arrangement_2::Curve_handle							Curve_handle;
typedef Arrangement_2::Halfedge_handle						Edge_handle;
typedef Arrangement_2::Vertex_handle						Vertex_handle;
typedef Arrangement_2::Halfedge_around_vertex_circulator	Edge_circulator;
typedef CGAL::Container_from_circulator<Edge_circulator>	Edge_container;
typedef Arrangement_2::Ccb_halfedge_circulator				Hole_circulator;
typedef CGAL::Container_from_circulator<Hole_circulator>	Hole_container;
typedef Arrangement_2::Ccb_halfedge_const_circulator		Ccb_const_circulator;
typedef CGAL::Container_from_circulator<Ccb_const_circulator>	Ccb_edge_container;
typedef CGAL::_Bezier_cache<Nt_traits>						Bezier_cache;

typedef std::vector<Bezier_curve_2>							Bezier_set;
typedef std::vector<X_monotone_curve_2>						Monotone_set;
typedef std::vector<Curve_handle>							Handle_set;
typedef std::vector<Edge_handle>							Edge_handle_set;
typedef std::vector<Point>									Point_set;

// An arrangement observer, used to receive notifications of face splits and
// to update the indices of the newly created faces.
class Face_index_observer : public CGAL::Arr_observer<Arrangement_2>
{
private:
	int n_faces;          // The current number of faces.
public:
	Face_index_observer(Arrangement_2& arr) :
		CGAL::Arr_observer<Arrangement_2>(arr),
		n_faces(0)
	{
		CGAL_precondition(arr.is_empty());
		arr.unbounded_face()->set_data(0);
		n_faces++;
	}
	virtual void after_split_face(Face_handle /* old_face */,
		Face_handle new_face, bool)
	{
		// Assign index to the new face.
		new_face->set_data(n_faces);
		n_faces++;
	}
};

// my typedefs
typedef std::vector<NSVGshape*>		Shape_set;
typedef std::vector<int>			Shape_indices;

// removes duplicates from a vector
template <typename Type>
void remove_duplicate(std::vector<Type> vec) {
	std::sort(vec.begin(), vec.end());
	vec.erase(unique(vec.begin(), vec.end()), vec.end());
}


// finds the index of a given iterator in its list
template <class Iterator>
int get_index_in_arrangement(Iterator object, Iterator begin)
{
	typedef typename Iterator::value_type T;
	return std::abs(std::distance(object, begin));
}

typedef std::_Vector_const_iterator<std::_Vector_val<std::_Simple_types<Point>>> Point_const_iterator;

Point_const_iterator find_point(Point_const_iterator& first,
	Point_const_iterator& last,
	const Point& p);

Arrangement_2::Curve_iterator find_curve(Arrangement_2::Curve_iterator& first,
	Arrangement_2::Curve_iterator& last,
	const Bezier_curve_2& c);

Arrangement_2::Face_iterator find_face(Arrangement_2::Face_iterator& first,
	Arrangement_2::Face_iterator& last,
	const Arrangement_2::Face_const_handle& f);

bool vertex_is_intersection(const Point& p, Arrangement_2& arr);
