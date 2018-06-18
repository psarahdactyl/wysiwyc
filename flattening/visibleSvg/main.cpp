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
//#include "make_monotone.h"
#include "compute_subcurves.h"
#include "flatten.h"


int main(int argc, char *argv[])
{

	const char* filename = (argc > 1) ? argv[1] : "C:/Users/sak/source/repos/wysiwyc/visibleSvg/files/svg-two-intersections.svg"; //"svg-shapes-filled.svg";

	// Each NSVGshape in the Shape_set has a corresponding (index-wise) 
	// Bezier_shape in a Bezier_set
	// Construct the arrangement.
	Arrangement_2 arr;
	Handle_set handles;
	Shape_set shapes;
	Bezier_set curves;
	Shape_indices indices;

	// Reads in an svg file and fills the shape and Bezier sets
	read_svg(filename, shapes, curves, indices);
	make_arrangement(arr, curves, handles);
	print_arrangement(arr);

	std::cout << "number of shapes total: " << shapes.size() << std::endl;

	
	// Makes each Bezier curve into a set of monotone curves
	//make_monotone(curves, monotone_curves, indices, monotone_indices);
	//std::cout << "number of monotone curves total: " << monotone_curves.size() << std::endl;

	// Each curve will be split again if it intersects with another
	Monotone_set subcurves;
	// Computes subcurves
	//compute_subcurves(curves, subcurves);

	flatten(arr, shapes, handles, indices);

	return 0;
}
