#include "write_svg.h"

inline bool is_nearly_equal(double x, double y)
{
	const double epsilon = 1e-5;
	return std::abs(x - y) <= epsilon * std::abs(x);
}

inline double calculate_slope(Control_point a, Control_point b)
{
	double rise = b.y().doubleValue() - a.y().doubleValue();
	double run = b.x().doubleValue() - a.x().doubleValue();
	double slope = 0;
	if (run != 0.0)
		slope = rise / run;
	else
		slope = rise;
	return slope;
}

inline bool is_a_line(Bezier_curve_2 b)
{
	double slope_01 = calculate_slope(b.control_point(0), b.control_point(1));
	double slope_12 = calculate_slope(b.control_point(1), b.control_point(2));
	double slope_23 = calculate_slope(b.control_point(2), b.control_point(3));

	if (is_nearly_equal(slope_01, slope_12) && is_nearly_equal(slope_12, slope_23))
		return true;

	return false;
}

void write_svg(const char* filename,
	const int width,
	const int height,
	const Shape_set& shapes,
	const Handle_set& handles,
	const Shape_indices& indices,
	const Eigen::SparseMatrix<int, Eigen::RowMajor>& winding_numbers,
	Arrangement_2& arr)
{
	std::cout << "writing out new svg" << std::endl;

	// initialize document
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
	decl->SetValue("xml version='1.0' encoding='utf-8'");
	doc.InsertFirstChild(decl);

	// make root node
	tinyxml2::XMLElement* svg_root = doc.NewElement("svg");

	std::string view_box = "0 0 " + std::to_string(width) + " " + std::to_string(height);
	std::string enable = "new 0 0 " + std::to_string(width) + " " + std::to_string(height);

	svg_root->SetAttribute("version", "1.0");
	svg_root->SetAttribute("xmlns", "http://www.w3.org/2000/svg");
	svg_root->SetAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
	svg_root->SetAttribute("viewBox", view_box.c_str());
	svg_root->SetAttribute("enable-background", enable.c_str());
	svg_root->SetAttribute("xml:space", "preserve");
	doc.InsertEndChild(svg_root);

	// add elements to svg

	// edges for strokes
	Arrangement_2::Halfedge_const_iterator it;
	for (it = arr.edges_begin(); it != arr.edges_end(); ++it)
	{
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
				std::string str_paths = "M";
				str_paths += std::to_string(bezier.control_point(0).x().doubleValue()) + "," +
					std::to_string(bezier.control_point(0).y().doubleValue()) + " C";
				for (int i = 1; i < bezier.number_of_control_points(); i++)
				{
					str_paths += std::to_string(bezier.control_point(i).x().doubleValue()) + "," +
						std::to_string(bezier.control_point(i).y().doubleValue()) + " ";
				}
				std::cout << str_paths << std::endl;
				edge->SetAttribute("d", str_paths.c_str());
				edge->SetAttribute("stroke", "#231F20");
				edge->SetAttribute("width", "2");
				edge->SetAttribute("fill", "none");
				svg_root->InsertEndChild(edge);
			}
			else
			{
				// if edge is a straight line
				if (is_a_line(bezier))
				{
					tinyxml2::XMLElement* edge = doc.NewElement("line");

					Traits_2::Rational start(0);
					Traits_2::Rational end(1);
					Rat_point_2 p0 = bezier(start);
					Rat_point_2 p1 = bezier(end);
					std::string x1 = std::to_string(p0.x().doubleValue());
					std::string y1 = std::to_string(p0.y().doubleValue());
					std::string x2 = std::to_string(p1.x().doubleValue());
					std::string y2 = std::to_string(p1.y().doubleValue());
					
					edge->SetAttribute("x1", x1.c_str());
					edge->SetAttribute("y1", y1.c_str());
					edge->SetAttribute("x2", x2.c_str());
					edge->SetAttribute("y2", y2.c_str());
					edge->SetAttribute("stroke", "#231F20");
					edge->SetAttribute("width", "2");
					edge->SetAttribute("fill", "none");
					svg_root->InsertEndChild(edge);
				}
				else // extract bezier curve from range of monotone curve
				{
					
					//double t = t0 + t1*u - t0*u
					//bezier.x_polynomial
				}
			}
			//edge->SetText()
		}
			
	}
	/*
	// faces for fills
	Arrangement_2::Face_const_iterator face;
	for (face = arr.faces_begin(); face != arr.faces_end(); ++face)
	{
		if (!face->is_unbounded())
		{
			// traverse the face's outer ccb
			Ccb_const_circulator outer_ccb = face->outer_ccb();
			Ccb_edge_container face_edges(outer_ccb);
			Ccb_edge_container::iterator edge = face_edges.begin();

			// face index
			int face_index = edge->face()->data();

			tinyxml2::XMLElement* transparent_edge = doc.NewElement("path");
			std::string str_paths = "M";

			// for edges in face
			for (edge = face_edges.begin(); edge != face_edges.end(); ++edge)
			{
				X_monotone_curve_2 edge_curve = edge->curve();
				Bezier_curve_2 bezier = edge_curve.supporting_curve();
				std::pair<double, double> parameter_range = edge_curve.parameter_range();

				int shape_index;
				Arrangement_2::Halfedge_const_handle e = edge->twin();
				find_shape_index_from_edge(arr, e, shapes, handles, indices, shape_index);

				double t0 = parameter_range.first;
				double t1 = parameter_range.second;

				if (edge == face_edges.begin())
				{
					str_paths += std::to_string(bezier.control_point(0).x().doubleValue()) + "," +
						std::to_string(bezier.control_point(0).y().doubleValue()) + " C";
				}
				else 
				{
					str_paths += "C";
				}
				// go through control points
				for (int i = 1; i < bezier.number_of_control_points(); i++)
				{
					str_paths += std::to_string(bezier.control_point(i).x().doubleValue()) + "," +
						std::to_string(bezier.control_point(i).y().doubleValue()) + " ";
				}

				
			}
			// close path
			str_paths += "Z";
			std::cout << str_paths << std::endl;

			// add path to svg
			transparent_edge->SetAttribute("d", str_paths.c_str());
			transparent_edge->SetAttribute("stroke", "#fff000");
			transparent_edge->SetAttribute("width", "2");
			transparent_edge->SetAttribute("fill", "#231F20");
			float opacity = (float)winding_numbers.row(face_index).sum() / 10;
			transparent_edge->SetAttribute("opacity", std::to_string(opacity).c_str());

			svg_root->InsertEndChild(transparent_edge);
		}
	}
	*/
	doc.SaveFile(filename);
}

/*
void write_svg(const char* filename,
	const int width,
	const int height,
	const Shape_set& shapes,
	const Handle_set& handles,
	const Shape_indices& indices,
	Arrangement_2& arr)
{
	// initialize document
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
	decl->SetValue("xml version='1.0' encoding='utf-8'");
	doc.InsertFirstChild(decl);

	// make root node
	tinyxml2::XMLElement* svg_root = doc.NewElement("svg");

	std::string view_box = "0 0 " + std::to_string(width) + " " + std::to_string(height);
	std::string enable = "new 0 0 " + std::to_string(width) + " " + std::to_string(height);

	svg_root->SetAttribute("version", "1.0");
	svg_root->SetAttribute("xmlns", "http://www.w3.org/2000/svg");
	svg_root->SetAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
	svg_root->SetAttribute("viewBox", view_box.c_str());
	svg_root->SetAttribute("enable-background", enable.c_str());
	svg_root->SetAttribute("xml:space", "preserve");
	doc.InsertEndChild(svg_root);

	// add elements to svg

	// edges for strokes
	Arrangement_2::Halfedge_const_iterator it;
	for (it = arr.edges_begin(); it != arr.edges_end(); ++it)
	{
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
				std::string str_paths = "M";
				str_paths += std::to_string(bezier.control_point(0).x().doubleValue()) + "," +
					std::to_string(bezier.control_point(0).y().doubleValue()) + " C";
				for (int i = 1; i < bezier.number_of_control_points(); i++)
				{
					str_paths += std::to_string(bezier.control_point(i).x().doubleValue()) + "," +
						std::to_string(bezier.control_point(i).y().doubleValue()) + " ";
				}
				std::cout << str_paths << std::endl;
				edge->SetAttribute("d", str_paths.c_str());
				edge->SetAttribute("stroke", "#231F20");
				edge->SetAttribute("width", "2");
				edge->SetAttribute("fill", "none");
				svg_root->InsertEndChild(edge);
			}
			else
			{
				// if edge is a straight line
				if (is_a_line(bezier))
				{
					tinyxml2::XMLElement* edge = doc.NewElement("line");

					Traits_2::Rational start(0);
					Traits_2::Rational end(1);
					Rat_point_2 p0 = bezier(start);
					Rat_point_2 p1 = bezier(end);//bezier.x_polynomial
					std::string x1 = std::to_string(p0.x().doubleValue());
					std::string y1 = std::to_string(p0.y().doubleValue());
					std::string x2 = std::to_string(p1.x().doubleValue());
					std::string y2 = std::to_string(p1.y().doubleValue());

					edge->SetAttribute("x1", x1.c_str());
					edge->SetAttribute("y1", y1.c_str());
					edge->SetAttribute("x2", x2.c_str());
					edge->SetAttribute("y2", y2.c_str());
					edge->SetAttribute("stroke", "#231F20");
					edge->SetAttribute("width", "2");
					edge->SetAttribute("fill", "none");
					svg_root->InsertEndChild(edge);
				}
				else
				{
					// extract bezier curve from range of monotone curve
					//double t = t0 + t1*u - t0*u

				}
			}
			//edge->SetText()
		}

	}

	// faces for fills
	Arrangement_2::Face_const_iterator face;
	for (face = arr.faces_begin(); face != arr.faces_end(); ++face)
	{
		if (!face->is_unbounded())
		{
			// traverse the face's outer ccb
			Ccb_const_circulator outer_ccb = face->outer_ccb();
			Ccb_edge_container face_edges(outer_ccb);
			Ccb_edge_container::iterator edge = face_edges.begin();

			// face index
			int face_index = edge->face()->data();

			tinyxml2::XMLElement* transparent_edge = doc.NewElement("path");
			std::string str_paths = "M";

			// for edges in face
			for (edge = face_edges.begin(); edge != face_edges.end(); ++edge)
			{
				X_monotone_curve_2 edge_curve = edge->curve();
				Bezier_curve_2 bezier = edge_curve.supporting_curve();
				std::pair<double, double> parameter_range = edge_curve.parameter_range();

				int shape_index;
				Arrangement_2::Halfedge_const_handle e = edge->twin();
				find_shape_index_from_edge(arr, e, shapes, handles, indices, shape_index);

				double t0 = parameter_range.first;
				double t1 = parameter_range.second;
				// if shape fill is not empty
				if (shapes[shape_index]->fill.color != 0)
				{
					
					str_paths += std::to_string(bezier.control_point(0).x().doubleValue()) + "," +
						std::to_string(bezier.control_point(0).y().doubleValue()) + " C";
					for (int i = 1; i < bezier.number_of_control_points(); i++)
					{
						str_paths += std::to_string(bezier.control_point(i).x().doubleValue()) + "," +
							std::to_string(bezier.control_point(i).y().doubleValue()) + " ";
					}
					std::cout << str_paths << std::endl;
					transparent_edge->SetAttribute("d", str_paths.c_str());
					transparent_edge->SetAttribute("stroke", "none");
					transparent_edge->SetAttribute("width", "2");
					transparent_edge->SetAttribute("fill", "#231F20");
					float opacity = (float)winding_numbers.row(face_index).sum() / 10;
					transparent_edge->SetAttribute("opacity", std::to_string(opacity).c_str());

					svg_root->InsertEndChild(transparent_edge);
				}
			}
		}
	}

	doc.SaveFile(filename);
}
*/