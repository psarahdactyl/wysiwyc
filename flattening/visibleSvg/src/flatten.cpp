#include "flatten.h"

typedef std::_Vector_iterator<std::_Vector_val<std::_Simple_types<Point>>> Point_iterator;

Point_iterator find_point(Point_iterator& first, 
	Point_iterator& last, 
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

bool vertex_is_intersection(const Point& p, Point_set& intersections)
{
	return find_point(intersections.begin(), intersections.end(), p) != intersections.end();
}

void find_comparable_shapes(const Vertex_handle& first,
	const Vertex_handle& second,
	const Arrangement_2::Curve_iterator& cit,
	const Shape_indices& indices,
	Arrangement_2& arr,
	Shape_indices& source_comparable_indices,
	Shape_indices& target_comparable_indices)
{
	Edge_circulator ec_from_source = first->incident_halfedges();
	Edge_circulator ec_from_target = second->incident_halfedges();
	Edge_container source_edges(ec_from_source);
	Edge_container target_edges(ec_from_target);
	Edge_container::iterator iheit;
	
	auto find_adjacent = [&] (auto edges, Shape_indices& comparable_indices)
	{
		for (iheit = edges.begin(); iheit != edges.end(); ++iheit)
		{
			X_monotone_curve_2 curve = iheit->curve();
			Bezier_curve_2 b_curve = curve.supporting_curve();

			int incident_curve_index = std::distance(find_curve(arr.curves_begin(), arr.curves_end(), iheit->curve().supporting_curve()), cit);
			int shape_index = indices[incident_curve_index];
			comparable_indices.push_back(shape_index);
		}
	};

	// adjacent shapes to source vertex of edge
	find_adjacent(source_edges, source_comparable_indices);

	// adjacent shapes to target vertex of edge
	find_adjacent(target_edges, target_comparable_indices);
	
}

bool should_remove_compare_shapes(const int& shape_index, const int& c_index, const Shape_set& shapes, Arrangement_2 arr)
{
	NSVGshape* s = shapes[shape_index];
	NSVGshape* c = shapes[c_index];

	unsigned int stroke_color = s->stroke.color;
	unsigned int fill_color = s->fill.color;
	std::cout << "\t stroke color id " << stroke_color << " from shape " << shape_index << std::endl;
	std::cout << "\t fill color id " << fill_color << " from shape " << shape_index << std::endl;

	if (c_index < shape_index)
	// if shape we're comparing (c_index) is in front of the shape that the edge belongs to (shape_index)
	{
		if (c->fill.color != 0)
		// if shape we're comparing is opaque
		{
			return true;
		}
		return false;
	}
	else
	// if shape we're comparing (c_index) is in behind the shape that the edge belongs to (shape_index)
	{
		if (s->fill.color == 0)
		// if shape edge belongs to is transparent
		{
			return true;
		}
		return false;
	}
}

void check_edge_removability(const Edge_handle& eh,
	Point_set& intersections,
	const Arrangement_2::Curve_iterator& cit,
	const Shape_set& shapes,
	const Shape_indices& indices,
	Arrangement_2& arr,
	Edge_handle_set& removable)
{
	// must mod by the number of curves so that distance is in correct range
	int curve_index = std::distance(cit, arr.curves_begin()) % arr.number_of_curves(); 
	int shape_index = indices[curve_index];

	int intersection_count = 0;
	std::vector<bool> should_remove_edge;

	Hole_container ccb(eh->ccb());
	Hole_container::iterator ccit = ccb.begin();

	// go through connected component boundary collecting links

	// until we are "out" of the intersection points
	// i.e. the target vertex of an edge is not an intersection 
	// !(vertex_is_intersection(ccit->target()->point(), intersections))
	for (ccit = ccb.begin(); ccit != ccb.end(); ++ccit)
	{
		Vertex_handle first = eh->source();
		Vertex_handle second = eh->target();
		Point v1 = first->point();
		Point v2 = second->point();

		// if target is intersection
		if (vertex_is_intersection(v2, intersections) && intersection_count == 0)
		{
			std::cout << "START THE CHAIN" << std::endl;
			intersection_count++;
		}

		if ( (vertex_is_intersection(v1, intersections) || vertex_is_intersection(v2, intersections))
			&& intersection_count > 0)
		{
			intersection_count++;
		}

		if (intersection_count > 0 && intersection_count <= 2)
		{
			// find all comparable shapes adjacent to each vertex of the edge
			Shape_indices source_comparable_indices;
			Shape_indices target_comparable_indices;
			Shape_indices comparable_indices;

			find_comparable_shapes(first, second, cit, indices, arr,
				source_comparable_indices, target_comparable_indices);

			std::cout << "source COMPARABLE INDICES SIZE: " << source_comparable_indices.size() << std::endl;
			std::cout << "target COMPARABLE INDICES SIZE: " << target_comparable_indices.size() << std::endl;

			std::sort(source_comparable_indices.begin(), source_comparable_indices.end());
			std::sort(target_comparable_indices.begin(), target_comparable_indices.end());

			// union of source and target vertex comparable shapes
			std::set_union(source_comparable_indices.begin(),
				source_comparable_indices.end(),
				target_comparable_indices.begin(),
				target_comparable_indices.end(),
				std::back_inserter(comparable_indices));

			std::cout << "COMPARABLE INDICES SIZE: " <<comparable_indices.size() << std::endl;

			auto it = std::adjacent_find(comparable_indices.begin(), comparable_indices.end());
			Shape_indices common_indices;
			while (it != comparable_indices.end())
			{
				int i = std::distance(comparable_indices.begin(), it);
				std::cout << "adjacent find at index " << i << std::endl;
				common_indices.push_back(comparable_indices[i]);
				it = std::adjacent_find(comparable_indices.begin(), comparable_indices.end(), std::greater<int>());
			}
			std::cout << "COMMON INDICES SIZE: " << common_indices.size() << std::endl;

			// compare with all comparable shapes
			for (int c_index : comparable_indices)
				should_remove_edge.push_back(should_remove_compare_shapes(shape_index, c_index, shapes, arr));

			// if any of the shapes are opaque and blocking the current edge, we should remove it
			if (std::any_of(should_remove_edge.begin(), should_remove_edge.end(), [](bool v) { return v; }))
				removable.push_back(eh);
		}

		// if source is intersection
		if ((vertex_is_intersection(v1, intersections) || vertex_is_intersection(v2, intersections))
			&& intersection_count == 2)
		{
			std::cout << "END THE CHAIN" << std::endl << std::endl;
			intersection_count = 0;
			break;
		}
		
	}

}

void handle_isolated_and_holes(const Shape_set& shapes, const Shape_indices& indices, Arrangement_2& arr)
{
	Arrangement_2::Face_iterator fit;
	for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit)
	{
		if (fit->number_of_holes() > 0) // there is a hole in the face
		{
			// find face color
			Hole_container outer(fit->outer_ccb());
			Hole_container::iterator outer_hcit = outer.begin();
			int face_curve_index = std::abs(std::distance(find_curve(arr.curves_begin(), arr.curves_end(), outer_hcit->curve().supporting_curve()), arr.curves_begin()));
			unsigned int face_color = shapes[indices[face_curve_index]]->fill.color;

			if (face_color != 0) // if face color is not transparent
			{
				Arrangement_2::Hole_iterator hit;
				// handle the holes
				for (hit = fit->holes_begin(); hit != fit->holes_end(); ++hit)
				{
					// find inner ccb face color
					Hole_circulator hc = *hit;
					Hole_container hole_edges(hc);
					Hole_container::iterator hcit = hole_edges.begin();
					int hole_curve_index = std::abs(std::distance(find_curve(arr.curves_begin(), arr.curves_end(), hcit->curve().supporting_curve()), arr.curves_begin()));
					unsigned int hole_color = shapes[indices[hole_curve_index]]->fill.color;

					if (hole_curve_index > face_curve_index) // if the hole is behind the face
					{
						for (hcit = hole_edges.begin(); hcit != hole_edges.end(); ++hcit)
						{
							Edge_handle edge_to_remove = hcit->twin();
							arr.remove_edge(edge_to_remove);
						}
					}
				}
			}
		}

		if (fit->number_of_isolated_vertices() > 0)
		{
			Arrangement_2::Isolated_vertex_iterator ivit;
			// remove the isolated vertices
			for (ivit = fit->isolated_vertices_begin(); ivit != fit->isolated_vertices_end(); ++ivit)
				arr.remove_isolated_vertex(ivit);
		}
	}
}

void flatten(Arrangement_2& arr, Shape_set& shapes, Handle_set& handles, Shape_indices& indices)
{
	Traits_2 tr;
	Point_set int_pts;

	// compute intersection points in the arrangement
	CGAL::compute_intersection_points(arr.curves_begin(), arr.curves_end(), std::back_inserter(int_pts), false, tr);
	std::cout << "correct number of intersections: " << int_pts.size() << std::endl;

	Point_set::iterator ipit;

	for (ipit = int_pts.begin(); ipit != int_pts.end(); ++ipit)
		std::cout << *ipit << std::endl;
	

	//std::cout << "curve colors: " << std::endl;
	Arrangement_2::Curve_iterator cit;

	Edge_handle_set removable;

	// go through each curve in the arrangement
	for (cit = arr.curves_begin(); cit != arr.curves_end(); ++cit)
	{
		std::cout << "-------" << std::endl;

		if (arr.number_of_induced_edges(cit) > 1) // if the number of induced edges is more than one, then we care about it.
												  // it was split by an intersection
		{
			//std::cout << "curve id: " << cit->id() << std::endl;
			Arrangement_2::Induced_edge_iterator ieit;

			// go through each induced edge
			for (ieit = arr.induced_edges_begin(cit); ieit != arr.induced_edges_end(cit); ++ieit)
			{
				Edge_handle eh = *ieit;

				// checks if an edge, or a link of edges is removable, populates the removable vector
				check_edge_removability(eh, int_pts, cit, shapes, indices, arr, removable);

			}

			
			std::cout << "-------" << std::endl;
			std::cout << "removing " << removable.size() << " edges from the arrangement" << std::endl;

			for (int i = 0; i < removable.size(); i++)
			{
				CGAL::remove_edge(arr, removable[i]);
			}

			removable.clear();
			
		}
	}

	std::cout << "-------" << std::endl;

	

	std::cout << "cleaning up isolated vertices and holes" << std::endl;


	std::cout << "-------" << std::endl;
}