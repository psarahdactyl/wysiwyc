#include "in_on_out.h"

// determines whether the centroid of a given segment is
// in, on, or outside of a given shape
// inside = 0
// on = 1
// outside = 2
int in_on_out(const Segment& seg, const Segment_set& intersecting_segs, const Point& intersection, const Segment_set& shape)
{
	std::cout << "INSIDE OUTSIDE TESTING" << std::endl;
	std::cout << "testing segment " << seg << std::endl;

	// find bounding box of shape
	CGAL::Bbox_2 bounds = bbox_2(shape.begin(), shape.end());

	// find centroid of segment
	Point centroid = CGAL::barycenter(seg.source(), 1, seg.target(), 1);

	// find if bounding boxes overlap
	if (CGAL::do_overlap(bounds, centroid.bbox()))
	{
		// find closest point to the centroid on the shape
		Segment i_seg = intersecting_segs[0];
		std::cout << "intersecting segment " << i_seg << std::endl;
		K::Construct_projected_point_2 project;
		Point proj = project(i_seg.supporting_line(), centroid);

		// make polygon to get orientation
		std::vector<Point> shape_points;
		for (Segment s : shape)
		{
			if (!already_in_set(shape_points, s.source()))
				shape_points.push_back(s.source());
			if (!already_in_set(shape_points, s.target()))
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

			//std::cout << "triangle area " << centroid_triangle.area() << std::endl;
			//std::cout << "polygon area " << shape_polygon.area() << std::endl;
			tri_area = centroid_triangle.area();
			poly_area = shape_polygon.area();

			std::cout << "triangle area sign " << CGAL::sign(tri_area) << std::endl;
			std::cout << "polygon area sign " << CGAL::sign(poly_area) << std::endl;

			Ray int_proj(intersection, proj);
			// horizontal case
			if (int_proj.is_horizontal())
			{
				std::cout << "horizontal" << std::endl;

				CGAL::Lazy_exact_nt<CGAL::Gmpq> int_proj_slope = int_proj.direction().dx();
				CGAL::Lazy_exact_nt<CGAL::Gmpq> seg_slope = i_seg.direction().dx();
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
			std::cout << "proj == intersection" << std::endl;
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