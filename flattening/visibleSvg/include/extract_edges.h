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

void extract_edges(Arrangement_2& arr,
	const Shape_set& shapes,
	const Handle_set& handles,
	const Shape_indices& indices,
	Eigen::SparseMatrix<int, Eigen::RowMajor>& winding_numbers);


