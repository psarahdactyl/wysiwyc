#include "assign_winding_number.h"

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

	int curve_index = get_index_in_arrangement<Arrangement_2::Curve_const_iterator>(found_curve, begin_curves);

	shape_index = indices[curve_index];
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
	Eigen::SparseMatrix<int, Eigen::RowMajor>& winding_numbers)
{
	std::stack<Arrangement_2::Face_const_iterator> stack;
	std::stack<int> parents; // ints are indices to faces in arrangement

	Arrangement_2::Hole_const_iterator inner_ccb;
	Arrangement_2::Face_const_iterator fit = arr.unbounded_face();

	stack.push(fit);
	parents.push(-1);

	while (!stack.empty())
	{
		std::cout << "-------" << std::endl;

		Arrangement_2::Face_const_iterator current_face = stack.top();
		stack.pop();
		int parent_face_index = parents.top();
		parents.pop();

		int first_face_index = current_face->data();

		if (!has_been_visited(first_face_index, visited))
		{
			mark_as_visited(first_face_index, visited);

			if (current_face->is_unbounded())
			{
				// going through inner ccbs 
				for (inner_ccb = current_face->holes_begin(); inner_ccb != current_face->holes_end(); ++inner_ccb)
				{
					Ccb_const_circulator ccb = *inner_ccb;
					Ccb_edge_container ccb_edges(ccb);
					Ccb_edge_container::iterator edge = ccb_edges.begin();

					// 
					for (edge; edge != ccb_edges.end(); ++edge)
					{
						Arrangement_2::Face_const_handle face;
						face = edge->twin()->face();
						int face_index = face->data();

						if (!has_been_visited(face_index, visited))
						{
							// add face to stack
							std::cout << "pushing face " << face_index << std::endl;
							stack.push(face);
							parents.push(first_face_index);
						}

					}
				}
			}
			else
			{
				Arrangement_2::Face_const_handle face = current_face;

				// traverse the face's outer ccb
				Ccb_const_circulator outer_ccb = face->outer_ccb();
				Ccb_edge_container face_edges(outer_ccb);
				Ccb_edge_container::iterator edge = face_edges.begin();

				// face index
				int face_index = edge->face()->data();

				// for edges in face
				for (edge = face_edges.begin(); edge != face_edges.end(); ++edge)
				{
					Arrangement_2::Halfedge_const_handle edge_twin = edge->twin();

					// if both faces are not unbounded
					if (!(edge->face()->is_unbounded()) && !(edge_twin->face()->is_unbounded()))
					{
						int edge_face_index = edge_twin->face()->data();

						if (!has_been_visited(edge_face_index, visited))
						{
							std::cout << "pushing face " << edge_face_index << std::endl;
							stack.push(edge_twin->face());
							parents.push(face_index);
						}
					}
				}
				for (edge = face_edges.begin(); edge != face_edges.end(); ++edge)
				{
					Arrangement_2::Halfedge_const_handle edge_twin = edge->twin();

					int edge_face_index = edge_twin->face()->data();

					if (parent_face_index == edge_face_index)
					{
						int shape_index;
						find_shape_index_from_edge(arr, edge_twin, shapes, indices, shape_index);

						// copy row from parent_face's row to current face's row
						winding_numbers.row(face_index) = winding_numbers.row(parent_face_index);
						std::cout << "after copying row " << parent_face_index << " to row " << face_index << std::endl;
						for (int k = 0; k < winding_numbers.rows(); k++)
						{
							auto row = winding_numbers.row(k);
							std::cout << row << std::endl;
						}

						// flip current face's shape bit
						winding_numbers.coeffRef(face_index, shape_index) = !winding_numbers.coeffRef(face_index, shape_index);
						std::cout << "after flipping bit " << face_index << ", " << shape_index << std::endl;
						for (int k = 0; k < winding_numbers.rows(); k++)
						{
							auto row = winding_numbers.row(k);
							std::cout << row << std::endl;
						}

						break;
					}
				}
			}

		}
	}

	std::cout << "FINAL MATRIX:" << std::endl;
	for (int k = 0; k < winding_numbers.rows(); k++)
	{
		auto row = winding_numbers.row(k);
		std::cout << row << std::endl;
	}
}