#include "flatten.h"


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

			Arrangement_2::Curve_iterator begin = arr.curves_begin();
			Arrangement_2::Curve_iterator end = arr.curves_end();
			Arrangement_2::Curve_const_iterator found;

			found = find_curve(begin, end, b_curve);

			int incident_curve_index = get_index_in_arrangement<Arrangement_2::Curve_const_iterator>(found, cit) % (arr.number_of_curves()-1);
			int shape_index = indices[incident_curve_index];
			comparable_indices.push_back(shape_index);
		}
	};

	// adjacent shapes to source vertex of edge
	find_adjacent(source_edges, source_comparable_indices);

	// adjacent shapes to target vertex of edge
	find_adjacent(target_edges, target_comparable_indices);
	
}

void handle_isolated_vertices(Arrangement_2& arr)
{
	Arrangement_2::Face_iterator fit;
	for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit)
	{
		if (fit->number_of_isolated_vertices() > 0)
		{
			Arrangement_2::Isolated_vertex_iterator ivit;
			// remove the isolated vertices
			for (ivit = fit->isolated_vertices_begin(); ivit != fit->isolated_vertices_end(); ++ivit)
				arr.remove_isolated_vertex(ivit);
		}
	}
}

void find_shape_index_from_edge(Arrangement_2& arr, 
	const Arrangement_2::Halfedge_const_handle& edge,
	const Shape_set& shapes, 
	const Shape_indices& indices, 
	int& shape_index)
{
	Arrangement_2::Curve_const_iterator begin_curves = arr.curves_begin();
	Arrangement_2::Curve_const_iterator end_curves = arr.curves_end();
	Arrangement_2::Curve_const_iterator found_curve;
	found_curve = find_curve(arr.curves_begin(), arr.curves_end(), edge->curve().supporting_curve());

	int curve_index = get_index_in_arrangement<Arrangement_2::Curve_const_iterator>(found_curve, begin_curves) % (arr.number_of_curves()-1);

	shape_index = indices[curve_index];
}

void find_face_in_arrangement(Arrangement_2& arr, const Arrangement_2::Face_const_handle& face, int& face_index)
{
	Arrangement_2::Face_iterator begin = arr.faces_begin();
	//Arrangement_2::Face_iterator end = arr.faces_end();
	//Arrangement_2::Face_iterator found;
	//found = find_face(begin, end, face);
	face_index = get_index_in_arrangement<Arrangement_2::Face_const_iterator>(face, begin) % (arr.number_of_faces()-1);
}

void mark_as_visited(const int& index, std::vector<int>& visited)
{
	visited[index] = 1;
}

bool has_been_visited(const int& index, std::vector<int>& visited)
{
	if (visited[index] == 1)
		return true;
	return false;
}

void assign_winding_numbers(Arrangement_2& arr, 
	const Shape_set& shapes, 
	const Shape_indices& indices, 
	std::vector<int>& visited,
	Eigen::SparseMatrix<int>& winding_numbers)
{
	std::stack<Arrangement_2::Face_const_iterator> stack;

	Arrangement_2::Hole_const_iterator inner_ccb;
	Arrangement_2::Face_const_iterator fit = arr.unbounded_face();

	stack.push(fit);

	while (!stack.empty())
	{
		std::cout << "-------" << std::endl;
		Arrangement_2::Face_const_iterator current_face = stack.top();
		int first_face_index;
		find_face_in_arrangement(arr, current_face, first_face_index);

		if (!has_been_visited(first_face_index, visited))
		{
			mark_as_visited(first_face_index, visited);

			stack.pop();

			Ccb_const_circulator ccb;
			Arrangement_2::Face_const_handle face;

			if (current_face->is_unbounded())
			{
				// going through inner ccbs 
				for (inner_ccb = current_face->holes_begin(); inner_ccb != current_face->holes_end(); ++inner_ccb)
				{
					ccb = *inner_ccb;
					Ccb_edge_container ccb_edges(ccb);
					Ccb_edge_container::iterator edge = ccb_edges.begin();

					// find the first bounded face (face in a closed shape)
					for (edge; edge != ccb_edges.end(); ++edge)
					{
						if (!(edge->twin()->face()->is_unbounded()))
							break;
					}
					// this will be the face we use
					face = edge->twin()->face();
					int face_index;
					find_face_in_arrangement(arr, face, face_index);
					if (!has_been_visited(face_index, visited))
					{
						// add face to stack
						stack.push(face);
					}
				}
			}
			else
			{
				face = current_face;

				// traverse the face's outer ccb
				Ccb_const_circulator outer_ccb = face->outer_ccb();
				Ccb_edge_container face_edges(outer_ccb);
				Ccb_edge_container::iterator edge = face_edges.begin();

				// shape index
				int crossed_edge_shape_index;
				find_shape_index_from_edge(arr, edge->twin(), shapes, indices, crossed_edge_shape_index);

				// face index
				int crossed_face_index;
				find_face_in_arrangement(arr, edge->face(), crossed_face_index);

				// update winding number
				winding_numbers.coeffRef(crossed_face_index, crossed_edge_shape_index) = 1;

				// for edges in face
				for (edge; edge != face_edges.end(); ++edge)
				{
					Arrangement_2::Halfedge_const_handle edge_twin = edge->twin();

					// if both faces are not unbounded
					if (!(edge->face()->is_unbounded()) && !(edge_twin->face()->is_unbounded()))
					{
						int face_edge_shape_index;
						find_shape_index_from_edge(arr, edge_twin, shapes, indices, face_edge_shape_index);

						if (crossed_edge_shape_index != face_edge_shape_index)
						{
							int new_face_index;
							find_face_in_arrangement(arr, edge_twin->face(), new_face_index);

							// update winding number
							winding_numbers.coeffRef(new_face_index, face_edge_shape_index) = 1;

							if (!has_been_visited(new_face_index, visited))
							{
								std::cout << "crossed edge shape index: " << crossed_edge_shape_index << std::endl;
								std::cout << "face edge shape index: " << face_edge_shape_index << std::endl;

								// update stack
								stack.push(edge_twin->face());
							}
						}
					}
				}
			}

		}
	}
	

	for (int k = 0; k < winding_numbers.rows(); k++)
	{
		auto row = winding_numbers.row(k);
		std::cout << row << std::endl;
	}
}

void flatten(Arrangement_2& arr, Shape_set& shapes, Handle_set& handles, Shape_indices& indices)
{

	// visited vector is the size (number of faces), all zeros -- change to one if visited 
	std::vector<int> visited(arr.number_of_faces(), 0);
	// winding number matrix is the size (number of faces x number of shapes)
	Eigen::SparseMatrix<int> winding_numbers(arr.number_of_faces(), shapes.size());
	assign_winding_numbers(arr, shapes, indices, visited, winding_numbers);
	

	/*
	std::cout << "-------" << std::endl;

	

	std::cout << "cleaning up isolated vertices" << std::endl;


	std::cout << "-------" << std::endl;
	*/
}