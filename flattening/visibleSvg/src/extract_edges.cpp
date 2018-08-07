#include "extract_edges.h"



void extract_edges(Arrangement_2& arr,
	const Shape_set& shapes,
	const Handle_set& handles,
	const Shape_indices& indices,
	Eigen::SparseMatrix<int, Eigen::RowMajor>& winding_numbers)
{
	Edge_handle_set removable;
	//std::vector<Arrangement_2::Halfedge_handle> removable;

	Arrangement_2::Halfedge_iterator edge;
	for (edge = arr.edges_begin(); edge != arr.edges_end(); ++edge)
	{
		X_monotone_curve_2 edge_curve = edge->curve();
		Bezier_curve_2 bezier = edge_curve.supporting_curve();

		int shape_index;
		find_shape_index_from_edge(arr, edge, shapes, handles, indices, shape_index);

		int face_right_index = edge->face()->data();
		int face_left_index = edge->twin()->face()->data();

		auto winding_num_right = winding_numbers.row(face_right_index);
		auto winding_num_left = winding_numbers.row(face_left_index);

		// XOR rows
		Eigen::RowVectorXi xor = winding_num_right - winding_num_left;
		xor.cwiseAbs();

		// AND rows
		Eigen::RowVectorXi and;
		and = winding_num_left.binaryExpr(winding_num_right, [](const int a, const int b) { return (bool)a & (bool)b; });

		int and_index = 0;
		for (int i = 0; i < and.size(); i++)
		{
			if (and(i) == 1)
			{
				and_index = i;
				break;
			}
		}

		// get non-zero index i.e. shape index
		for (int i = 0; i < xor.size(); i++)
		{
			if (xor(i) != 0)
			{
				if (i == shape_index) // make sure
				{
					if (i < and_index)
					{
						removable.push_back(edge);
					}
				}
			}
		}

	}

	std::cout << "removing " << removable.size() << " edges from the arrangement" << std::endl;
	for (int i = 0; i < removable.size(); i++)
	{
		/*
		Traits_2 tr;
		auto are_mergeable = tr.are_mergeable_2_object;
		auto merge_edge = tr.merge_2_object;

		Edge_handle e1 = removable[i];
		Edge_handle e2;

		if (i + 1 < removable.size())
			e2 = removable[i + 1];
		else
			e2 = removable[0];

		if (are_mergeable(e1, e2))
			merge_edge(e1, e2);
		*/

		CGAL::remove_edge(arr, removable[i]);
		//arr.remove_edge(removable[i], false, false);
	}
}