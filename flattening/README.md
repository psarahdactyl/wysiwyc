# flattening
Retrieving what you actually want to cut based on what you see. Takes a given SVG and output a new SVG where only the visible strokes/fills are present.

## Project:
Using CGAL's Epeck, we are able to remove unnecessary curves/lines/geometry in a 2D vector graphic design.

## Dependencies:
* CGAL 4.12
* Boost 6.7
* tinyxml2
* NanoSVG (a header only library, which is already included in this repository)

## Prerequisite installation:
This assumes you have at least CMake 3.9.4.

Install the above dependecies. Follow the [CGAL installation instructions](https://doc.cgal.org/latest/Manual/installation.html) exactly. As part of this installation, you should have installed Boost. When you install TinyXML2, [clone/download the repository](https://github.com/leethomason/tinyxml2) to the same place where you installed Boost. On Windows, this is `C:\local\`.

## Included files: 
The `CMakeLists.txt` file setups up the cmake build routine for the program.
There are example SVG files in `visible_strokes/files`. 

## Compilation

This and all following assignments will follow a typical cmake/make build
routine. Starting in this directory, issue:

    cd visible_strokes/build
    cmake ../..

If you are using Mac or Linux, then issue:

    make

If you are using Windows, then running `cmake ..` should have created a Visual Studio solution file
called `visible_strokes.sln` that you can open and build from there. Building the project will generate an .exe file.

## Usage

Once built, you can execute the assignment from inside the `build/` using 

    ./visible_strokes [path to input file]
    
or

    ./visible_strokes.exe [path to input file]
    

