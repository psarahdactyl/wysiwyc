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

// CGAL includes and typedefs
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

// boost includes
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>

// Eigen includes
//#include <Eigen/Dense>

//typedef boost::graph_traits<Graph>							Graph;
//typedef boost::graph_traits<Graph>::vertex_descriptor		Vertex;

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

typedef CGAL::Arrangement_with_history_2<Traits_2>			Arrangement_2;
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


// A property map that reads/writes the information to/from the extended
// face.
template <typename Arrangement, class Type> class Extended_face_property_map {
public:
	typedef typename Arrangement::Face_handle       Face_handle;
	// Boost property type definitions.
	typedef boost::read_write_property_map_tag      category;
	typedef Type                                    value_type;
	typedef value_type&                             reference;
	typedef Face_handle                             key_type;
	// The get function is required by the property map concept.
	friend reference get(const Extended_face_property_map& /* map */,
		key_type key)
	{
		return key->data();
	}
	// The put function is required by the property map concept.
	friend void put(Extended_face_property_map /* map */,
		key_type key, value_type val)
	{
		key->set_data(val);
	}
};

// my typedefs
typedef std::vector<NSVGshape*>		Shape_set;
typedef std::vector<int>			Shape_indices;

// Flattens a vector of vectors (https://tinyurl.com/ycdwpz4r)
template <template<typename...> class R = std::vector,
	typename Top,
	typename Sub = typename Top::value_type>
	R<typename Sub::value_type> flatten_vector(Top const& all)
{
	using std::begin;
	using std::end;

	R<typename Sub::value_type> accum;

	for (auto& sub : all)
		accum.insert(end(accum), begin(sub), end(sub));

	return accum;
}


template <typename Type>
void remove_duplicate(std::vector<Type> vec) {
	std::sort(vec.begin(), vec.end());
	vec.erase(unique(vec.begin(), vec.end()), vec.end());
}

