Smallest Enclosing Ball of Points/Balls Code

This distribution contains C++ code to calculate the smallest enclosing
ball of points and balls.  It compiles as is on the GNU g++ compiler
version 2.6.3, but it should compile on most C++ compilers, and should be
easy (but perhaps tedious) to translate into ansi or k&r C.  The main
function is "EnclosingBall" and documentation on its parameters are
contained in the header file "ball.h".  This function requires code to
calculate the singular value decomposition SVD.  The current implementation
uses the numerical recipes version.  However, since that code is
copyrighted, I can not include it in this distribution.

The numerical recipes code is from the book:
Numerical Recipes in C, The Art of Scientific Computation
by William H. Press, Saul A. Teukolsky, William T. Vellerling, and
Brain P. Flannery
Cambridge University Press

The numerical recipes files needed to compile this as is are:
nrutil.h
nr.h
complex.h
svdcmp.c
svbksb.c

The code in this distribution should be:
ball.h		Prototypes and explanation of parameters of EnclosingBall()
ball.cc		The C++ code (although not object oriented).
tball.cc	A driver test program that reads a file and prints the output.
Makefile	A simple Makefile to compile an executable "tball"

1.tst		A simple sample file to test "tball" on points
2.tst		A simple sample file to test "tball" on balls

There is a lot of SVD/matrix computing code on the net.  See the following
for some C++ libraries:
http://www.cis.ohio-state.edu/hypertext/faq/usenet/C++-faq/libraries/faq.html

There is also a lot of good fortran (yuk) code for this purpose.  However,
my code is based on assumptions about numerical recipes such as arrays
starting at index 1 instead of starting at 0 like most other C code.
Therefore, modification will be necessary to use another package.

See the following reference for documentation on the algorithms used.
However, my implementation is non-recursive, so it may be difficult to find
correspondence with the original algorithm.  (In retrospect, I probably
should written it recursively so it would be more understandable.)

Emo Welzl
"Smallest enclosing disks (balls and ellipsoids)"
in "New Results and New Trends in Computer Science"
H. Maurer, Editor
Lecture Notes in Computer Science 555 (1991)
pg. 359-370
Also available as a Tech Report B 91-09
Institut f\:ur Informatik
Freie Universit\:at Berlin
Arnimallee 2-6, W 100 Berlin 33, Germany
email: emo@inf.fu-berlin.de

Another good reference for further reading on complexity bounds is:

Jiri Matousek, Micha Sharir, and Emo Welzl
"A Subexponential Bound for Linear Programming"
8th Annual ACM Conference on Computational Geometry

The ideas used in writing this code came from code provided by Bernd
Gaertner to find the minimum ball enclosed by points based on the former
above (email: gaertner@inf.fu-berlin.du).

You may use this code for any purpose, commercial or otherwise, but all
standard disclaimers apply.  In other words, use it totally at your own
risk.

Known Bugs/Warnings: If you use this code only in 2-3D, you probably will
never encounter any problems.  However, since I have exercised this code
extensively (100K+ calls) in high dimensions (36D and 100D), I have run
into a few numeric problems, but I believe the algorithms themselves are
correct.  These seem to be caused by roundoff or near singularities in
matrices.  In order to avoid near singularities, the smallest ball radius
can be off by as much as 1.5%.  The main numeric problem was the SVD
routines not converging.  (However, in a given function call, the chances
are very small of this happening even when using high dimensional data.)
The solution may be either to go to double precision arithmetic or to use
another SVD package, since I heard numerical recipies SVD routine might not
be the best SVD package available.

If you have questions or bug reports regarding this code, send me email.  I
am also interested in how this code is being used, so email explaining your
application would also be appreciated.

Dave White
dwhite@cs.ucsd.edu
http://vision.ucsd.edu/~dwhite

Acknowledgement: I also thank Nina Amenta from the Computational Geometry
center for all her help.
