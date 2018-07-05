#include "read_svg.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

bool make_geometry(float* p, Bezier_curve_2& c, bool& closed)
{
	std::cout << "making geometry" << std::endl;
	// Read the curves one by one
	Traits_2 tr;
	Rat_kernel ker;
	auto equal = ker.equal_2_object();

	// Read the current curve.
	const std::vector<Rat_point_2> points = { Rat_point_2(p[0], p[1]), 
												Rat_point_2(p[2], p[3]), 
												Rat_point_2(p[4], p[5]), 
												Rat_point_2(p[6], p[7]) };

	c = Bezier_curve_2( points.begin(), points.end() );

	if (equal(c.control_point(0), c.control_point(c.number_of_control_points() - 1)))
		closed = true;

	return true;
}

void make_curves(NSVGimage* image, Shape_set& shapes, Bezier_set& curves, Shape_indices& indices)
{

	NSVGshape* shape;
	NSVGpath* path;

	int shape_number = 0;
	for (shape = image->shapes; shape != NULL; shape = shape->next)
	{
		for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
		{
			Bezier_curve_2 B;
			bool success;

			for (int i = 0; i < path->npts - 1; i += 3)
			{
				float* p = &(path->pts)[i * 2];
				bool closed = false;
				success = make_geometry(p, B, closed);
				if (success)
				{
					curves.push_back(B);
					indices.push_back(shape_number);
				}

			}

			std::cout << "made path" << std::endl;
		}
		shapes.push_back(shape);
		shape_number++;
	}
}

void read_svg(const char* filename, std::vector<NSVGshape*>& shapes, Bezier_set& curves, Shape_indices& indices)
{
	NSVGimage* g_image = NULL;

	g_image = nsvgParseFromFile(filename, "px", 96.0f);
	std::cout << "parsed original svg" << std::endl;
	
	make_curves(g_image, shapes, curves, indices);
	std::cout << "made curves\n" << "-------" << std::endl << std::endl;

	// --------------- PRINTING ---------------
	/*
	for (int i = 0; i < curves.size(); i++)
	{
		Bezier_shape c_shape = curves.at(i);
		for (int j = 0; j < c_shape.size(); j++)
		{
			Bezier_curve_2 c = c_shape[j];
			for (int k = 0; k < c.number_of_control_points(); k++)
				std::cout << "shape " << i + 1 << std::endl <<
				"curve " << j + 1 <<  std::endl <<
				"control point " << k + 1 << ": " << c.control_point(k) << std::endl;
		}
	}
	*/
}
