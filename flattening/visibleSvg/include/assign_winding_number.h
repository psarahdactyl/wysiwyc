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
//#include "face_index_observer.h"

void mark_as_visited(const int& index, std::vector<int>& visited);

bool has_been_visited(const int& index, std::vector<int>& visited);

void assign_winding_numbers(Arrangement_2& arr,
	const Shape_set& shapes,
	const Shape_indices& indices,
	std::vector<int>& visited,
	Eigen::SparseMatrix<int, Eigen::RowMajor>& winding_numbers);

