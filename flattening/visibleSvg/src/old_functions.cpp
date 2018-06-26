
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