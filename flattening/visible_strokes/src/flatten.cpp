#include "flatten.h"
#include "write_svg.h"

/*
template<typename T>
inline void remove(vector<T> & v, const T & item)
{
v.erase(std::remove(v.begin(), v.end(), item), v.end());
}
*/

struct Intersector {
	typedef CGAL::cpp11::result_of<K::Intersect_2(Segment, Segment)>::type result_type;
	const Segment& s;
	K::Intersect_2 intersect;
	Intersector(const Segment& seg) : s(seg) {}
	result_type
		operator() (const Segment& other) const
	{
		return intersect(s, other);
	}
};

// determines whether the centroid of a given segment is
// in, on, or outside of a given shape
// inside = 0
// on = 1
// outside = 2
int in_on_out(const Segment& seg, const Segment_set& intersecting_segs, const Point& intersection, const Segment_set& shape)
{
	// find bounding box of shape
	CGAL::Bbox_2 bounds = bbox_2(shape.begin(), shape.end());

	// find centroid of segment
	Point centroid = CGAL::barycenter(seg.source(), 1, seg.target(), 1);

	// find if bounding boxes overlap
	if (CGAL::do_overlap(bounds, centroid.bbox()))
	{
		// find closest point to the centroid on the shape
		Segment i_seg = intersecting_segs[0];
		K::Construct_projected_point_2 project;
		Point proj = project(i_seg.supporting_line(), centroid);

		// make polygon to get orientation
		std::vector<Point> shape_points;
		for (Segment s : shape)
		{
			if (!(std::find(shape_points.begin(), shape_points.end(), s.source()) != shape_points.end()))
				shape_points.push_back(s.source());
			if (!(std::find(shape_points.begin(), shape_points.end(), s.target()) != shape_points.end()))
				shape_points.push_back(s.target());
		}
		Polygon shape_polygon(shape_points.begin(), shape_points.end());
		
		CGAL::Lazy_exact_nt<CGAL::Gmpq> tri_area;
		CGAL::Lazy_exact_nt<CGAL::Gmpq> poly_area;

		// if the projection is not equal to the intersection point
		// will check if the slopes have the same sign
		std::cout << "intersection point: " << intersection << " projection: " << proj << std::endl;
		if (intersection != proj)
		{
			// find if centroid is inside shape
			Point_set triangle_points = { intersection, proj, centroid };
			Polygon centroid_triangle(triangle_points.begin(), triangle_points.end());

			std::cout << "triangle area " << centroid_triangle.area() << std::endl;
			std::cout << "polygon area " << shape_polygon.area() << std::endl;
			tri_area = centroid_triangle.area();
			poly_area = shape_polygon.area();

			Ray int_proj(intersection, proj);
			// horizontal case
			if (int_proj.is_horizontal())
			{
				std::cout << "horizontal" << std::endl;

				CGAL::Lazy_exact_nt<CGAL::Gmpq> int_proj_slope = int_proj.direction().dx();
				CGAL::Lazy_exact_nt<CGAL::Gmpq> seg_slope = i_seg.direction().dx();;
				if (!(CGAL::sign(int_proj_slope) == CGAL::sign(seg_slope)))
				{
					tri_area *= -1;
					std::cout << "triangle area sign " << CGAL::sign(tri_area) << std::endl;
					std::cout << "polygon area sign " << CGAL::sign(poly_area) << std::endl;
				}
			}
			// vertical case
			else if (int_proj.is_vertical())
			{
				std::cout << "vertical" << std::endl;

				CGAL::Lazy_exact_nt<CGAL::Gmpq> int_proj_slope = int_proj.direction().dy();
				CGAL::Lazy_exact_nt<CGAL::Gmpq> seg_slope = i_seg.direction().dy();
				std::cout << CGAL::sign(int_proj_slope) << " " << CGAL::sign(seg_slope) << std::endl;
				if (!(CGAL::sign(int_proj_slope) == CGAL::sign(seg_slope)))
				{
					tri_area *= -1;
					std::cout << "triangle area sign " << CGAL::sign(tri_area) << std::endl;
					std::cout << "polygon area sign " << CGAL::sign(poly_area) << std::endl;
				}
			}
			// regular case
			else
			{
				std::cout << "normal" << std::endl;

				CGAL::Lazy_exact_nt<CGAL::Gmpq> int_proj_y = int_proj.direction().dy(); 
				CGAL::Lazy_exact_nt<CGAL::Gmpq> seg_y =	i_seg.direction().dy();
				CGAL::Lazy_exact_nt<CGAL::Gmpq> int_proj_x = int_proj.direction().dx();
				CGAL::Lazy_exact_nt<CGAL::Gmpq> seg_x =	i_seg.direction().dx();
				if (!(CGAL::sign(int_proj_y) == CGAL::sign(seg_y)) 
					&& !(CGAL::sign(int_proj_x) == CGAL::sign(seg_x)))
				{
					tri_area *= -1;
					std::cout << "triangle area sign " << CGAL::sign(tri_area) << std::endl;
					std::cout << "polygon area sign " << CGAL::sign(poly_area) << std::endl;
				}
			}
		}
		else
		{
			// find if centroid is inside shape
			Point_set triangle_points = { intersection, i_seg.target(), centroid };
			Polygon centroid_triangle(triangle_points.begin(), triangle_points.end());
			tri_area = centroid_triangle.area();
			poly_area = shape_polygon.area();
		}
		
		if(CGAL::sign(tri_area) == CGAL::sign(poly_area))
		{
			// inside
			return 0;
		}
		else if (i_seg.has_on(centroid))
		{
			// on
			return 1;
		}
		// outside
		return 2;
		
	}
	else
	{
		// outside
		return 2;
	}

	return 0;
}


void split_segment(const Segment& old_seg, Intersection_set& intersections, Split_intersection_set& splits)
{
	std::sort(intersections.begin(),
		intersections.end(),
		[](auto &a, auto &b) { return a.first < b.first; });

	for (int i = 0; i <= intersections.size(); i++)
	{
		Segment_set segs;
		if (i == 0)
		{
			segs.push_back(intersections[i].second);
			Segment s = Segment(old_seg.source(), intersections[i].first);
			splits.push_back(Split_intersection(s, segs));
		}
		else if (i == intersections.size())
		{
			segs.push_back(intersections[i - 1].second);
			Segment s = Segment(intersections[i - 1].first, old_seg.target());
			splits.push_back(Split_intersection(s, segs));
		}
		else
		{
			// TODO: check this. which seg to push back?
			segs.push_back(intersections[i].second);
			segs.push_back(intersections[i - 1].second);
			Segment s = Segment(intersections[i - 1].first, intersections[i].first);
			splits.push_back(Split_intersection(s, segs));
		}
		segs.clear();
	}
}

void decide_to_keep(const Split_intersection_set& splits, 
	const Intersection_set& intersections, 
	const Segment_set& shape, 
	std::vector<int>& decisions)
{
	for (int i = 0; i < splits.size(); i++)
	{
		if (i >= intersections.size())
		{
			decisions.push_back(in_on_out(splits[i].first, splits[i].second, intersections[i-1].first, shape));
		}
		else 
		{
			decisions.push_back(in_on_out(splits[i].first, splits[i].second, intersections[i].first, shape));
		}
		std::cout << decisions[i] << std::endl;
	}
}

void compare_2_shapes(Segment_set& s1, 
	Segment_set& s2, 
	Segment_set& visible_segments,
	const Shape_set& shapes, 
	const int shape_number,
	Shape_indices& indices)
{
	Shape shape = shapes[shape_number];
	bool intersected = false;
	// if new shape fill is not transparent
	if (shape->fill.color != 0)
	{
		// go through each segment of old shape
		for (int i = 0; i < s1.size(); i++)
		{
			Segment old_seg = s1[i];
			Intersection_set intersections;
			Segment new_seg;
			intersected = false;
			// go through each segment in the new shape
			for (int j = 0; j < s2.size(); j++)
			{
				new_seg = s2[j];
				if (CGAL::do_intersect(old_seg, new_seg))
				{
					intersected = true;
					auto result = intersection(old_seg, new_seg);
					if (const Segment* s = boost::get<Segment>(&*result))
					{
						std::cout << "segment " << *s << std::endl;
						intersections.push_back(Intersection(s->source(), new_seg));
						intersections.push_back(Intersection(s->target(), new_seg));
					}
					else
					{
						const Point* p = boost::get<Point>(&*result);
						std::cout << "point " << *p << std::endl;
						intersections.push_back(Intersection(*p, new_seg));
					}
				}
				// add NEW segments 
				if (!(std::find(visible_segments.begin(), visible_segments.end(), new_seg) != visible_segments.end()))
				{
					std::cout << "ADDING SEGMENT " << new_seg << std::endl;
					visible_segments.push_back(new_seg);
					indices.push_back(shape_number);
				}

			}
			if (intersected)
			{
				Split_intersection_set splits;
				split_segment(old_seg, intersections, splits);

				std::vector<int> decisions;
				decide_to_keep(splits, intersections, s2, decisions);

				intersections.clear();

				for (int k = 0; k < decisions.size(); k++)
				{
					int d = decisions[k];
					//std::cout << d << std::endl;
					if (d != 0)
					{
						visible_segments.push_back(splits[k].first);
						indices.push_back(shape_number - 1);
					}
				}
			}
			else
			{
				if (!(std::find(visible_segments.begin(), visible_segments.end(), old_seg) != visible_segments.end()))
				{
					visible_segments.push_back(old_seg);
					indices.push_back(shape_number - 1);
				}
			}
		}
	}
}

void flatten(const Shape_set& shapes,
	const Shape_segment_set& segments,
	Segment_set& visible_segments,
	Shape_indices& indices)
{
	Segment_set build;
	int shape_number = 0;
	for (Segment_set shape : segments)
	{
		if (shape_number == 0)
		{
			boost::range::push_back(build, shape);
			boost::range::push_back(visible_segments, shape);
			Shape_indices first_shape_indices(shape.size(), 0);
			boost::range::push_back(indices, first_shape_indices);
		}
		else
		{
			if (shape_number < segments.size() - 1)
			{
				visible_segments.clear();
			}

			compare_2_shapes(build, shape, visible_segments, shapes, shape_number, indices);
			build.clear();

			std::string filename = "C:/Users/sak/source/repos/wysiwyc/flattening/visible_strokes/files/intermediate";
			filename += std::to_string(shape_number);
			std::string ending = ".svg";
			filename += ending;
			write_svg(filename.c_str(), 620, 620, shapes, visible_segments, indices);

			boost::range::push_back(build, visible_segments);

		}

		shape_number++;
	}
}

/*
{
	Segment_set shape_to_add;
	int shape_start_index = 0;
	int previous_shape = 0;
	int current_shape = 0;
	int next_shape = 0;

	for (int i = 0; i < segments.size(); i++)
	{
		Segment segment = segments[i];
		current_shape = indices[i];
		
		Shape shape = shapes[current_shape];

		// get previous and next shape index
		if (i != 0)
		{
			previous_shape = indices[i - 1];
		}
		if (i != segments.size() - 1)
		{
			next_shape = indices[i + 1];
		}
		else
		{
			next_shape = -1;
		}
		
		// if we are on the first shape
		if (current_shape == 0)
		{
			// add the segment to the visible ones
			visible_segments.push_back(segment);
		}
		else if (current_shape > 0)
		{
			if (current_shape != previous_shape)
			{
				// we are on a new shape
				shape_start_index = i;

				// if we are NOT on the last shape
				if (next_shape != -1)
				{
					// if the current shape is only a single segment
					if (current_shape != next_shape)
					{
						shape_to_add.push_back(segment);
					}
					else 
					{
						// if indices are equal in this range, they belong to the same shape
						if (std::equal(indices.begin() + shape_start_index, indices.begin() + i, indices.begin() + shape_start_index))
						{
							// add to shape
							shape_to_add.push_back(segment);

						}
					}
				}
				// if we ARE on the last shape
				else
				{
					shape_to_add.push_back(segment);
				}
				// if the shape we were building is not empty
				if (!shape_to_add.empty())
				{
					// compare the shapes
					compare_2_shapes(visible_segments, shape_to_add, visible_segments);
					// start a new shape
					shape_to_add.clear();
				}

			}
		}


	}

}
*/


/*
float max = std::numeric_limits<float>::max();
Point min_proj(max, max);
for (int i = 0; i < intersecting_segs.size(); i++)
{
Segment i_seg = intersecting_segs[i];
K::Construct_projected_point_2 project;
Point proj = project(i_seg.supporting_line(), centroid);

if (!seg.has_on(proj))
{
proj = CGAL::compare_distance_to_point(proj, i_seg.source(), i_seg.target()) ==
CGAL::SMALLER ? i_seg.source() : i_seg.target();
}

// the closest to the centroid will be the min projection
min_proj = CGAL::compare_distance_to_point(centroid, proj, min_proj) ==
CGAL::SMALLER ? proj : min_proj;
}
*/