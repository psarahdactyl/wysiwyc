#include "assign_winding_number.h"

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
	const Handle_set& handles,
	const Shape_indices& indices,
	std::vector<int>& visited,
	Eigen::SparseMatrix<int, Eigen::RowMajor>& winding_numbers)
{

	std::queue<Arrangement_2::Face_const_iterator> queue;
	std::queue<int> parents; // ints are indices to faces in arrangement

	Arrangement_2::Hole_const_iterator inner_ccb;
	Arrangement_2::Face_const_iterator fit = arr.unbounded_face();

	queue.push(fit);
	parents.push(-1);

	while (!queue.empty())
	{
		std::cout << "-------" << std::endl;

		Arrangement_2::Face_const_iterator current_face = queue.front();
		queue.pop();
		int parent_face_index = parents.front();
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
							// add face to queue
							std::cout << "pushing face " << face_index << std::endl;
							queue.push(face);
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
				std::cout << "FACE NUMBER " << face_index << std::endl;

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
							queue.push(edge_twin->face());
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
						find_shape_index_from_edge(arr, edge_twin, shapes, handles, indices, shape_index);

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