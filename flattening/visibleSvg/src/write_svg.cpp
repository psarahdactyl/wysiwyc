#include "write_svg.h"

inline bool is_nearly_equal(double x, double y)
{
	const double epsilon = 1e-5;
	return std::abs(x - y) <= epsilon * std::abs(x);
}

inline bool is_a_line(Bezier_curve_2 b)
{
	//if(b.control_point(0).x() == b.)

	return false;
}

void write_svg(const char* filename,
	const Shape_set& shapes,
	const Handle_set& handles,
	const Shape_indices& indices,
	Arrangement_2& arr)
{
	// initialize document
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
	decl->SetValue("version='1.0' encoding='utf-8'");
	doc.LinkEndChild(decl);

	// make root node
	tinyxml2::XMLNode* svg_root = doc.NewElement("svg");
	//svg_root->
	doc.InsertFirstChild(svg_root);

	// add elements to svg
	// edges for strokes
	Arrangement_2::Halfedge_const_iterator it;
	for (it = arr.edges_begin(); it != arr.edges_end(); ++it)
	{
		// extract bezier curve from range of monotone curve
		X_monotone_curve_2 edge_curve = it->curve();
		Bezier_curve_2 bezier = edge_curve.supporting_curve();
		std::pair<double, double> parameter_range = edge_curve.parameter_range();

		int shape_index;
		find_shape_index_from_edge(arr, it, shapes, handles, indices, shape_index);

		double t0 = parameter_range.first;
		double t1 = parameter_range.second;
		// if shape stroke is not empty
		if (shapes[shape_index]->stroke.color != 0)
		{
			
			// if edge takes up whole curve
			if (is_nearly_equal(t0, 0.0) && is_nearly_equal(t1, 1.0))
			{
				tinyxml2::XMLElement* edge = doc.NewElement("path");
				edge->SetAttribute("d", shapes[shape_index]->paths);
			}
			else
			{
				// if edge is a straight line
				if (is_a_line(bezier))
				{
					tinyxml2::XMLElement* edge = doc.NewElement("line");

					Traits_2::Rational start(0);
					Traits_2::Rational end(1);
					Rat_point_2 p0 = bezier(end);
					Rat_point_2 p1 = bezier(start);//bezier.x_polynomial
					const char* x1 = p0.x().get_str().c_str();
					const char* y1 = p0.y().get_str().c_str();
					const char* x2 = p1.x().get_str().c_str();
					const char* y2 = p1.x().get_str().c_str();
					
					edge->SetAttribute("x1", x1);
					edge->SetAttribute("y1", y1);
					edge->SetAttribute("x2", x2);
					edge->SetAttribute("y2", y2);
					edge->SetAttribute("stroke", "stroke:rgb(255, 0, 0); stroke-width:2");
				}
				else
				{
					//double t = t0 + t1*u - t0*u

				}
			}
			//edge->SetText()
		}
			
	}

	// faces for fills
	
}