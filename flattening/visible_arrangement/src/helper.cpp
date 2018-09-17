#include "helper.h"


Point_const_iterator find_point(Point_const_iterator& first,
	Point_const_iterator& last,
	const Point& p)
{
	Bezier_cache c;
	while (first != last)
	{
		if ((*first).equals(p, c))
			return first;
		++first;
	}
	return last;
}

/*
Arrangement_2::Curve_iterator find_curve(Arrangement_2::Curve_iterator& first,
	Arrangement_2::Curve_iterator& last,
	const Bezier_curve_2& c)
{
	while (first != last)
	{
		if ((*first).is_same(c))
			return first;
		++first;
	}
	return last;
}
*/

int find_handle(Handle_set handles,
	const Bezier_curve_2& c)
{
	for (int i = 0; i < handles.size(); i++)
	{
		Bezier_curve_2 b = *(handles[i]);
		if (b.is_same(c))
		{
			return i;
		}
	}
	return -1;
}

Arrangement_2::Face_iterator find_face(Arrangement_2::Face_iterator& first,
	Arrangement_2::Face_iterator& last,
	const Arrangement_2::Face_const_handle& f)
{
	Bezier_cache c;
	
	while (first != last)
	{
		Ccb_const_circulator ccb_1;
		Ccb_const_circulator ccb_2;

		if (first->number_of_outer_ccbs() > 0 && f->number_of_outer_ccbs() > 0)
		{
			ccb_1 = first->outer_ccb();
			ccb_2 = f->outer_ccb();
		}
		else 
		{
			Arrangement_2::Hole_const_iterator inner_ccb_1 = first->holes_begin();
			ccb_1 = *inner_ccb_1; 

			Arrangement_2::Hole_const_iterator inner_ccb_2 = f->holes_begin();
			ccb_2 = *inner_ccb_2;
		}

		Ccb_edge_container ccb_edges_1(ccb_1);
		Ccb_edge_container ccb_edges_2(ccb_2);

		
		bool same = true;
		for (Ccb_edge_container::iterator it_1 = ccb_edges_1.begin(); it_1 != ccb_edges_1.end(); ++it_1)
		{
			for (Ccb_edge_container::iterator it_2 = ccb_edges_2.begin(); it_2 != ccb_edges_2.end(); ++it_2)
			{
				Point first_source = it_1->source()->point();
				Point first_target = it_1->target()->point();
				Point second_source = it_2->source()->point();
				Point second_target = it_2->target()->point();
				if (first_source.equals(second_source, c) && first_target.equals(second_target, c))
				{
					same = true;
				}
				else
				{
					same = false;
					break;
				}
			}
		}

		if (same)
			return first;
		++first;
	}
	return last;
}

bool vertex_is_intersection(const Point& p, Arrangement_2& arr)
{
	Traits_2 tr;
	Point_set intersections;

	// compute intersection points in the arrangement
	CGAL::compute_intersection_points(arr.curves_begin(), arr.curves_end(), std::back_inserter(intersections), false, tr);

	Point_set::iterator ipit;

	for (ipit = intersections.begin(); ipit != intersections.end(); ++ipit)
		std::cout << *ipit << std::endl;

	return find_point(intersections.begin(), intersections.end(), p) != intersections.end();
}

void find_shape_index_from_edge(Arrangement_2& arr,
	const Arrangement_2::Halfedge_const_handle& edge,
	const Shape_set& shapes,
	const Handle_set& handles,
	const Shape_indices& indices,
	int& shape_index)
{
	Handle_set::iterator found_curve;

	Arrangement_2::Originating_curve_iterator originating = arr.originating_curves_begin(edge);
	int curve_index = find_handle(handles, *originating);

	if (curve_index == indices.size())
	{
		shape_index = -1;
	}
	else
	{
		shape_index = indices[curve_index];
	}

}