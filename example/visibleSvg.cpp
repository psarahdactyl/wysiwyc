//
// Sarah Kushner
// May 2018
// 
// visibleSvg: a C++ program for reading in and parsing an SVG
// and outputting two SVGs:
//	- visible strokes
//	- visible fills
//
// NanoSVG for parsing files (some code is taken from their example1 demo)
// CGAL for polygon clipping
// GLFW for rendering
// using bad deprecated OpenGL for now - i.e. glBegin() and such
// 

// regular cpp includes
#include <cmath>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <iostream>
#include <string>

// c and nanoSVG includes
#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_ALL_COLOR_KEYWORDS	// Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION
#define _ALLOW_RTCc_IN_STL
#include "nanosvg.h"

// clipper includes
#include "clipper.hpp"

// GLFW includes
#include <GLFW/glfw3.h>

// CGAL includes and typedefs
#include <CGAL/Cartesian.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Arr_Bezier_curve_traits_2.h>
#include <CGAL/Gps_traits_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Timer.h>

typedef CGAL::CORE_algebraic_number_traits              Nt_traits;
typedef Nt_traits::Rational                             Rational;
typedef Nt_traits::Algebraic                            Algebraic;
typedef CGAL::Cartesian<Rational>                       Rat_kernel;
typedef CGAL::Cartesian<Algebraic>                      Alg_kernel;
typedef CGAL::Arr_Bezier_curve_traits_2<Rat_kernel, Alg_kernel, Nt_traits>
Traits_2;

typedef Rat_kernel::Point_2                             Rat_point_2;
typedef Traits_2::Curve_2                               Bezier_curve_2;
typedef Traits_2::X_monotone_curve_2                    X_monotone_curve_2;
typedef CGAL::Gps_traits_2<Traits_2>                    Gps_traits_2;
typedef Gps_traits_2::General_polygon_2                 Polygon_2;
typedef Gps_traits_2::General_polygon_with_holes_2      Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                 Polygon_set;

//typedef CGAL::Exact_predicates_exact_constructions_kernel K;

// globals i'm sorry
NSVGimage* g_image = NULL;

static unsigned char bgColor[4] = {205,202,200,255};
//static unsigned char lineColor[4] = { 0,160,192,255 };
static unsigned char lineColor[4] = { 0,0,0,255 };



static float distPtSeg(float x, float y, float px, float py, float qx, float qy)
{
	float pqx, pqy, dx, dy, d, t;
	pqx = qx-px;
	pqy = qy-py;
	dx = x-px;
	dy = y-py;
	d = pqx*pqx + pqy*pqy;
	t = pqx*dx + pqy*dy;
	if (d > 0) t /= d;
	if (t < 0) t = 0;
	else if (t > 1) t = 1;
	dx = px + t*pqx - x;
	dy = py + t*pqy - y;
	return dx*dx + dy*dy;
}

static void cubicBez(float x1, float y1, float x2, float y2,
					 float x3, float y3, float x4, float y4,
					 float tol, int level)
{
	float x12,y12,x23,y23,x34,y34,x123,y123,x234,y234,x1234,y1234;
	float d;
	
	if (level > 12) return;

	x12 = (x1+x2)*0.5f;
	y12 = (y1+y2)*0.5f;
	x23 = (x2+x3)*0.5f;
	y23 = (y2+y3)*0.5f;
	x34 = (x3+x4)*0.5f;
	y34 = (y3+y4)*0.5f;
	x123 = (x12+x23)*0.5f;
	y123 = (y12+y23)*0.5f;
	x234 = (x23+x34)*0.5f;
	y234 = (y23+y34)*0.5f;
	x1234 = (x123+x234)*0.5f;
	y1234 = (y123+y234)*0.5f;

	d = distPtSeg(x1234, y1234, x1,y1, x4,y4);
	if (d > tol*tol) {
		cubicBez(x1,y1, x12,y12, x123,y123, x1234,y1234, tol, level+1); 
		cubicBez(x1234,y1234, x234,y234, x34,y34, x4,y4, tol, level+1); 
	} else {
		glVertex2f(x4, y4);
	}
}

void drawPath(float* pts, int npts, char closed, float tol)
{
	int i;
	glBegin(GL_LINE_STRIP);
	glColor4ubv(lineColor);
	glVertex2f(pts[0], pts[1]);
	for (i = 0; i < npts-1; i += 3) {
		float* p = &pts[i*2];
		//std::cout << "i: " << i << std::endl;
		//std::cout << p[0] << " " << p[1] << " " << p[2] << " " << p[3] << " " << p[4] << " " << p[5] << " " << p[6] << " " << p[7] << std::endl;
		cubicBez(p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7], tol, 0);
	}
	if (closed) {
		glVertex2f(pts[0], pts[1]);
	}
	glEnd();
}


void drawControlPts(float* pts, int npts)
{
	int i;

	// Control lines
	/*
	glColor4ubv(lineColor);
	glBegin(GL_LINES);
	for (i = 0; i < npts-1; i += 3) {
		float* p = &pts[i*2];
		glVertex2f(p[0],p[1]);
		glVertex2f(p[2],p[3]);
		glVertex2f(p[4],p[5]);
		glVertex2f(p[6],p[7]);
	}
	glEnd();
	*/

	// Points
	glPointSize(6.0f);
	glColor4ubv(lineColor);

	glBegin(GL_POINTS);
	glVertex2f(pts[0],pts[1]);
	for (i = 0; i < npts-1; i += 3) {
		float* p = &pts[i*2];
		glVertex2f(p[6],p[7]);
	}
	glEnd();

	// Points
	/*
	glPointSize(3.0f);

	glBegin(GL_POINTS);
	glColor4ubv(bgColor);
	glVertex2f(pts[0],pts[1]);
	for (i = 0; i < npts-1; i += 3) {
		float* p = &pts[i*2];
		glColor4ubv(lineColor);
		glVertex2f(p[2],p[3]);
		glVertex2f(p[4],p[5]);
		glColor4ubv(bgColor);
		glVertex2f(p[6],p[7]);
	}
	glEnd();
	*/
}

bool makeGeometry(NSVGshape* shape, Polygon_with_holes_2& P)
{
	std::cout << "making geometry" << std::endl;
	// Read the curves one by one, and construct the general polygon these
	// curve form (the outer boundary and the holes inside it).
	Traits_2                    traits;
	Traits_2::Make_x_monotone_2 mk_x_monotone = traits.make_x_monotone_2_object();
	bool                        first = true;
	std::list<X_monotone_curve_2>  xcvs;
	Rat_kernel                  ker;
	Rat_kernel::Equal_2         equal = ker.equal_2_object();
	std::list<Polygon_2>        pgns;

	std::cout << "is this the problem?" << std::endl;
	Rat_point_2                 p_0;


	for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
	{
		for (int i = 0; i < path->npts - 1; i += 3)
		{
			std::cout << i << std::endl;
			float* p = &(path->pts)[i * 2];

			// Read the current curve and subdivide it into x-monotone subcurves.
			Bezier_curve_2                           B;
			std::list<CGAL::Object>                  x_objs;
			std::list<CGAL::Object>::const_iterator  xoit;
			X_monotone_curve_2                       xcv;

			std::vector<Rat_point_2> points = { Rat_point_2(p[0], p[1]), Rat_point_2(p[2], p[3]), Rat_point_2(p[4], p[5]), Rat_point_2(p[6], p[7]) };
			std::vector<float>::iterator i1;
			//std::copy(points.begin(), points.end(), std::back_inserter(B));
			B = Bezier_curve_2( points.begin(), points.end() );

			mk_x_monotone(B, std::back_inserter(x_objs));

			for (xoit = x_objs.begin(); xoit != x_objs.end(); ++xoit) {
				if (CGAL::assign(xcv, *xoit))
					xcvs.push_back(xcv);
			}

			// Check if the current curve closes a polygon, namely whether it target
			// point (the last control point) equals the source of the first curve in
			// the current chain.
			if (!first) {
				if (equal(p_0, B.control_point(B.number_of_control_points() - 1))) {
					// Push a new polygon into the polygon list. Make sure that the polygon
					// is counterclockwise oriented if it represents the outer boundary
					// and clockwise oriented if it represents a hole.
					Polygon_2          pgn(xcvs.begin(), xcvs.end());
					CGAL::Orientation  orient = pgn.orientation();

					if ((pgns.empty() && (orient == CGAL::CLOCKWISE)) ||
						(!pgns.empty() && (orient == CGAL::COUNTERCLOCKWISE)))
						pgn.reverse_orientation();

					pgns.push_back(pgn);
					xcvs.clear();
					first = true;
				}
			}
			else {
				// This is the first curve in the chain - store its source point.
				p_0 = B.control_point(0);
				first = false;
			}
		}
	}
	
	if (!xcvs.empty())
		return false;
	// Construct the polygon with holes.
	std::list<Polygon_2>::iterator     pit = pgns.begin();

	++pit;
	P = Polygon_with_holes_2(pgns.front(), pit, pgns.end());

	return true;

}

void flattenSVG(NSVGimage* g_image)
{
	NSVGshape* shape;
	NSVGpath* path;

	for (shape = g_image->shapes; shape != NULL; shape = shape->next) {
		Polygon_with_holes_2 P;
		makeGeometry(shape, P);
	}

}

void drawframe(GLFWwindow* window)
{
	int width = 0, height = 0;
	float view[4], cx, cy, hw, hh, aspect, px;
	NSVGshape* shape;
	NSVGpath* path;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Fit view to bounds
	cx = g_image->width*0.5f;
	cy = g_image->height*0.5f;
	hw = g_image->width*0.5f;
	hh = g_image->height*0.5f;

	if (width/hw < height/hh) {
		aspect = (float)height / (float)width;
		view[0] = cx - hw * 1.2f;
		view[2] = cx + hw * 1.2f;
		view[1] = cy - hw * 1.2f * aspect;
		view[3] = cy + hw * 1.2f * aspect;
	} else {
		aspect = (float)width / (float)height;
		view[0] = cx - hh * 1.2f * aspect;
		view[2] = cx + hh * 1.2f * aspect;
		view[1] = cy - hh * 1.2f;
		view[3] = cy + hh * 1.2f;
	}
	// Size of one pixel.
	px = (view[2] - view[1]) / (float)width;

	glOrtho(view[0], view[2], view[3], view[1], -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glColor4ub(255,255,255,255);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	// Draw bounds
	glColor4ub(0,0,0,64);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(g_image->width, 0);
	glVertex2f(g_image->width, g_image->height);
	glVertex2f(0, g_image->height);
	glEnd();

	for (shape = g_image->shapes; shape != NULL; shape = shape->next) {
		Polygon_with_holes_2 P;
		makeGeometry(shape, P);
		
		std::cout << "shape" << std::endl;
		for (path = shape->paths; path != NULL; path = path->next) {
			drawPath(path->pts, path->npts, path->closed, px * 1.5f);
			drawControlPts(path->pts, path->npts);
		}
		
	}

	glfwSwapBuffers(window);
}

void resizecb(GLFWwindow* window, int width, int height)
{
	// Update and render
	NSVG_NOTUSED(width);
	NSVG_NOTUSED(height);
	drawframe(window);
}

int main()
{
	/*
	GLFWwindow* window;
	const GLFWvidmode* mode;

	if (!glfwInit())
		return -1;
	

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwDefaultWindowHints();
    window = glfwCreateWindow(mode->width - 300, mode->height - 600, "SVG Preview", NULL, NULL);
	if (!window)
	{
		printf("Could not open window\n");
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, resizecb);
	glfwMakeContextCurrent(window);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	*/
	g_image = nsvgParseFromFile("../example/svg-test-simple.svg", "px", 96.0f);
	flattenSVG(g_image);

	/*
	if (g_image == NULL) {
		printf("Could not open SVG image.\n");
		glfwTerminate();
		return -1;
	}

	while (!glfwWindowShouldClose(window))
	{
		drawframe(window);
		glfwPollEvents();
	}
	*/
	nsvgDelete(g_image);

	//glfwTerminate();
	return 0;
}
