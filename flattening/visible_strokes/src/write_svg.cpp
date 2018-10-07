#include "write_svg.h"
#include <iomanip>

inline bool is_nearly_equal(double x, double y)
{
	const double epsilon = 1e-5;
	return std::abs(x - y) <= epsilon * std::abs(x);
}

void write_svg(const char* filename,
	const int width,
	const int height,
	const Shape_set& shapes,
	//const Shape_indices& indices,
	const Shape_segment_set& segments)
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
	// x="0px" y="0px"
	// width = "620px" height = "620px"
	svg_root->SetAttribute("x", "0px");
	svg_root->SetAttribute("y", "0px");
	svg_root->SetAttribute("width", (std::to_string(width) +"px").c_str());
	svg_root->SetAttribute("height", (std::to_string(height) + "px").c_str());
	svg_root->SetAttribute("viewBox", view_box.c_str());
	svg_root->SetAttribute("enable-background", enable.c_str());
	svg_root->SetAttribute("xml:space", "preserve");
	doc.InsertEndChild(svg_root);

	// add elements to svg
	for (int i = 0; i < segments.size(); i++)
	{
		tinyxml2::XMLElement* edge = doc.NewElement("line");
		Segment_set shape_segment = segments[i];
		//int shape_index = indices[i];
		Shape shape = shapes[i];

		for (int j = 0; j < shape_segment.size(); j++)
		{
			Segment segment = shape_segment[j];

			unsigned int c = shape->stroke.color;
			unsigned int color = ((c & 0xff0000) >> 16) | (c & 0x00ff00) | ((c & 0x0000ff) << 16);
			color &= 0x00ffffff;

			std::stringstream stream;
			stream << "#" << std::setfill('0') << std::setw(6) << std::hex << color;
			std::string hex_color = stream.str();

			Point p0 = segment.source();
			Point p1 = segment.target();

			std::string x1 = std::to_string(p0.x().exact().to_double());
			std::string y1 = std::to_string(p0.y().exact().to_double());
			std::string x2 = std::to_string(p1.x().exact().to_double());
			std::string y2 = std::to_string(p1.y().exact().to_double());

			edge->SetAttribute("x1", x1.c_str());
			edge->SetAttribute("y1", y1.c_str());
			edge->SetAttribute("x2", x2.c_str());
			edge->SetAttribute("y2", y2.c_str());
			edge->SetAttribute("stroke", hex_color.c_str());
			edge->SetAttribute("width", "2");
			edge->SetAttribute("fill", "none");
			svg_root->InsertEndChild(edge);

		}
	}
	doc.SaveFile(filename);
}

void write_svg(const char* filename,
	const int width,
	const int height,
	const Shape_set& shapes,
	const Segment_set& segments,
	const Shape_indices& indices)
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
	svg_root->SetAttribute("x", "0px");
	svg_root->SetAttribute("y", "0px");
	svg_root->SetAttribute("width", (std::to_string(width) + "px").c_str());
	svg_root->SetAttribute("height", (std::to_string(height) + "px").c_str());
	svg_root->SetAttribute("viewBox", view_box.c_str());
	svg_root->SetAttribute("enable-background", enable.c_str());
	svg_root->SetAttribute("xml:space", "preserve");
	doc.InsertEndChild(svg_root);

	// add elements to svg
	for (int i = 0; i < segments.size(); i++)
	{
		tinyxml2::XMLElement* edge = doc.NewElement("line");
		int shape_index = indices[i];
		std::cout << "index " << shape_index << std::endl;
		Shape shape = shapes[shape_index];

		Segment segment = segments[i];

		unsigned int c = shape->stroke.color;
		unsigned int color = ((c & 0xff0000) >> 16) | (c & 0x00ff00) | ((c & 0x0000ff) << 16);
		color &= 0x00ffffff;

		float alpha = shape->opacity;

		std::stringstream stream;
		stream << "#" << std::setfill('0') << std::setw(6) << std::hex << color;
		std::string hex_color = stream.str();

		Point p0 = segment.source();
		Point p1 = segment.target();

		std::string x1 = std::to_string(p0.x().exact().to_double());
		std::string y1 = std::to_string(p0.y().exact().to_double());
		std::string x2 = std::to_string(p1.x().exact().to_double());
		std::string y2 = std::to_string(p1.y().exact().to_double());

		std::string stroke_width = std::to_string(shape->strokeWidth);
		std::string opacity = std::to_string(alpha);

		edge->SetAttribute("x1", x1.c_str());
		edge->SetAttribute("y1", y1.c_str());
		edge->SetAttribute("x2", x2.c_str());
		edge->SetAttribute("y2", y2.c_str());
		edge->SetAttribute("stroke", hex_color.c_str());
		edge->SetAttribute("stroke-width", stroke_width.c_str());
		edge->SetAttribute("fill", "none");
		edge->SetAttribute("opacity", opacity.c_str());
		svg_root->InsertEndChild(edge);

		
	}
	doc.SaveFile(filename);
}