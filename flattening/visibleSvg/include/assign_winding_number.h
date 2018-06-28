//
// Sarah Kushner
// May 2018
// 
// a C++ program for reading in and parsing an SVG
// and outputting two SVGs:
//	- visible strokes
//	- visible fills
//
// NanoSVG for parsing files
// CGAL for polygon clipping


#include "helper.h"


void find_shape_index_from_edge(Arrangement_2& arr,
	const Arrangement_2::Halfedge_const_handle& edge,
	const Shape_set& shapes,
	const Shape_indices& indices,
	int& shape_index);

void mark_as_visited(const int& index, std::vector<int>& visited);

bool has_been_visited(const int& index, std::vector<int>& visited);

void assign_winding_numbers(Arrangement_2& arr,
	const Shape_set& shapes,
	const Shape_indices& indices,
	std::vector<int>& visited,
	Eigen::SparseMatrix<int, Eigen::RowMajor>& winding_numbers);

