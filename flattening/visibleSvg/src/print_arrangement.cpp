#include "print_arrangement.h"

void print_ccb(Arrangement_2::Ccb_halfedge_const_circulator circ)
{
	Arrangement_2::Ccb_halfedge_const_circulator curr = circ;
	std::cout << "(" << curr->source()->point() << ")";
	do {
		Arrangement_2::Halfedge_const_handle he = curr->ccb();
		std::cout << " [" << he->curve() << "] "
			<< "(" << he->target()->point() << ")";
	} while (++curr != circ);
	std::cout << std::endl;
}

void print_face(Arrangement_2::Face_const_handle f)
{
	// Print the outer boundary.
	if (f->is_unbounded())
		std::cout << "Unbounded face. " << std::endl;
	else {
		std::cout << "Outer boundary: ";
		print_ccb(f->outer_ccb());
	}
	// Print the boundary of each of the holes.
	Arrangement_2::Hole_const_iterator hi;
	int index = 1;
	for (hi = f->holes_begin(); hi != f->holes_end(); ++hi, ++index) {
		std::cout << " Hole #" << index << ": ";
		print_ccb(*hi);
	}
	// Print the isolated vertices.
	Arrangement_2::Isolated_vertex_const_iterator iv;
	for (iv = f->isolated_vertices_begin(), index = 1;
		iv != f->isolated_vertices_end(); ++iv, ++index) {
		std::cout << " Isolated vertex #" << index << ": "
			<< "(" << iv->point() << ")" << std::endl;
	}
}

void print_arrangement(Arrangement_2& arr)
{
	// --------------- PRINTING ---------------

	// Print the arrangement size.
	std::cout << "The arrangement size:" << std::endl
		<< "   V = " << arr.number_of_vertices()
		<< ",  E = " << arr.number_of_edges()
		<< ",  F = " << arr.number_of_faces() << std::endl;

	std::cout << "-------" << std::endl;

	// Print out the curves and the number of edges each one induces.
	Arrangement_2::Curve_iterator cit;
	std::cout << "The arrangement contains "
		<< arr.number_of_curves() << " curves:" << std::endl;

	for (cit = arr.curves_begin(); cit != arr.curves_end(); ++cit)
		std::cout << "Curve [" << *cit << "] induces "
		<< arr.number_of_induced_edges(cit) << " edge(s)." << std::endl;

	std::cout << "-------" << std::endl;

	// Print the arrangement edges, along with the list of curves that induce each edge.
	Arrangement_2::Edge_iterator eit;
	Arrangement_2::Originating_curve_iterator ocit;
	std::cout << "The arrangement is comprised of "
		<< arr.number_of_edges() << " edges:" << std::endl;


	for (eit = arr.edges_begin(); eit != arr.edges_end(); ++eit) 
	{
		std::cout << "[" << eit->curve() << "]. " << std::endl << "Originating curves: ";
		for (ocit = arr.originating_curves_begin(eit); ocit != arr.originating_curves_end(eit); ++ocit)
			std::cout << " [" << *ocit << "]" << std::flush;
		std::cout << std::endl << std::endl;
	}

	std::cout << "-------" << std::endl;
}
