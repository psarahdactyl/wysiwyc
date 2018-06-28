#include "flatten.h"
#include "assign_winding_number.h"

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



void flatten(Arrangement_2& arr, Shape_set& shapes, Handle_set& handles, Shape_indices& indices)
{

	// visited vector is the size (number of faces), all zeros -- changes to one if visited 
	std::vector<int> visited(arr.number_of_faces(), 0);
	// winding number matrix is the size (number of faces x number of shapes), all zeros, flips under certain con
	Eigen::SparseMatrix<int, Eigen::RowMajor> winding_numbers(arr.number_of_faces(), shapes.size());

	assign_winding_numbers(arr, shapes, indices, visited, winding_numbers);
	

	/*
	std::cout << "-------" << std::endl;

	std::cout << "cleaning up isolated vertices" << std::endl;
	handle_isolated_vertices(arr);

	std::cout << "-------" << std::endl;
	*/
}