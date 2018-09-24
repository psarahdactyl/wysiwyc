# wysiwyc
what you see is what you cut. 

## Project:
A Laser Cutting Visualization Prototype, and many experiments.

## People:
* Sarah Kushner - PhD Student at DGP, University of Toronto
* Arjun Chhabra - Undergraduate Research Assistant at DGP, University of Toronto
* Alec Jacobson - Assistant Professor at DGP, University of Toronto

## Directories:
* <tt>/flattening/</tt> <br/>
   A C++ project which takes in an SVG file and rids the design of invisible, redundant vectors. Uses CGAL, tinyxml and NanoSVG.
* <tt>/OverlapView/</tt> <br/>
   A Python program that utilizes Cairo to render an alpha composited view of overlaps in curves from an SVG file. Used to predict the number of times a laser passes through each point. <br/>
* <tt>/exploder_strokes_fills/</tt> <br/>
   A Python program that explodes a given SVG into separate strokes and fills and writes out a new SVG. <br/>

## Laser cutters:
:warning: Trotec Speedy 400 flex <br/>
:warning: Universal Laser Systems Versalaser

## To do
 - batch convert to SVG 1.0 with "CSS Properties: Style Attributes"
