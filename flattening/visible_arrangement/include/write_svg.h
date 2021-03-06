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

/*
void write_svg(const char* filename, 
	const int width, 
	const int height,
	const Shape_set& shapes, 
	const Handle_set& handles,
	const Shape_indices& indices, 
	Arrangement_2& arr);
*/

void write_svg(const char* filename,
	const int width,
	const int height,
	const Shape_set& shapes,
	const Handle_set& handles,
	const Shape_indices& indices,
	const Eigen::SparseMatrix<int, Eigen::RowMajor>& winding_numbers,
	Arrangement_2& arr);