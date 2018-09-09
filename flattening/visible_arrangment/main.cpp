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
// 

#include "read_svg.h"
#include "make_arrangement.h"
#include "print_arrangement.h"
#include "flatten.h"
#include "write_svg.h"


int main(int argc, char *argv[])
{

	const char* filename = (argc > 1) ? argv[1] : "C:/Users/sak/source/repos/wysiwyc/flattening/visibleSvg/files/debug/svg-shapes-white-fill.svg"; 
	//svg-overlap-edges-exact.svg"; //"svg-shapes-filled.svg";

	// Each NSVGshape in the Shape_set has a corresponding (index-wise) 
	// Bezier_shape in a Bezier_set
	// Construct the arrangement.
	Arrangement_2 arr;
	Face_index_observer obs(arr);
	Handle_set handles;
	Shape_set shapes;
	Bezier_set curves;
	Shape_indices indices;
	int width, height;

	// Reads in an svg file and fills the shape and Bezier sets
	read_svg(filename, shapes, curves, indices, width, height);
	make_arrangement(arr, obs, curves, handles);
	print_arrangement(arr);

	std::cout << "number of shapes total: " << shapes.size() << std::endl;

	Eigen::SparseMatrix<int, Eigen::RowMajor> winding_numbers;
	flatten(arr, shapes, handles, indices, winding_numbers);

	std::string new_filename = filename;
	std::string ending = "_flattened.svg";
	new_filename += ending;
	std::cout << new_filename << std::endl;

	write_svg(new_filename.c_str(), width, height, shapes, handles, indices, winding_numbers, arr);

	return 0;
}
