# flattening
Retrieving what you actually want to cut based on what you see. Takes a given SVG and output a new SVG where only the visible strokes/fills are present.

## Project
Using CGAL's Epeck, we are able to remove unnecessary curves/lines/geometry in a 2D vector graphic design.

## Dependencies
* CGAL 4.12
* Boost 6.7
* Eigen 3.3 (a header only library, which is included using git submodules)
* tinyxml2 (a header only library, which is already included in this repository)
* NanoSVG (also a header only library, which is already included in this repository)

## Operating Systems
Will test on Linux.
We only support Windows 64-bit. If you are using Windows, you will need Visual Studio to build.

## Prerequisite installation
This assumes you have at least CMake 3.9.4.

Install the above dependecies. Follow the [CGAL installation instructions](https://doc.cgal.org/latest/Manual/installation.html) **exactly**. (This [webpage](https://www.cgal.org/download/windows.html) is somehow better for the Windows installation instructions. In fact, here is the [link](https://github.com/CGAL/cgal/releases) to where you can download the CGAL setup exe for Windows.) As part of this installation, you should have installed Boost. 

When you download Eigen, [clone/download the repository](https://github.com/eigenteam/eigen-git-mirror) to the same place where you installed Boost. On Mac or Linux, this will probably be `/usr/include` or `/usr/local/include`. On Windows, this is `C:\local\`. If you are using Windows, make a new environment variable called `EIGEN3_DIR` and set it to `C:\local\[where you cloned Eigen]`.

## Included files
The `CMakeLists.txt` file sets up the cmake build routine for the program.
There are example SVG files in `visible_strokes/files`. 

## Compilation
Clone this repository using the following command:

    git clone --recursive http://github.com/psarahdactyl/wysiwyc.git

This follows a typical cmake/make build
routine. Starting in this directory, change to the build directory:

    cd visible_strokes/build
    
If you are using Windows, issue:

    cmake ../.. -T "host=x64" -A x64
    
Then, open the resulting Visual Studio solution file and build from there.

If you are using Mac or Linux, then issue:

    cmake ../..
    make

## Usage

Once built, you can execute the assignment from inside the `build/` using 

    ./visible_strokes [path to input file]
    
or

    ./visible_strokes.exe [path to input file]
    

