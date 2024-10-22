#ifndef _BALL_H
#include "ball.h"
#endif

#include <iostream.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  ulong ii, jj, num_dim, num_points;
  ulong repeat = 1;
  float **centers;
  float *out_center;
  float *radii;
  ulong *bound_arr;
  int doball;

  if (argc > 1)
    repeat = atoi(argv[1]);
  cin >> doball;
  if (!cin || (doball != 0 && doball != 1))
    repeat = 0;
  cin >> num_dim;
  if (!cin || num_dim < 2 || num_dim > (1 << 14))
    repeat = 0;
  cin >> num_points;
  if (!cin || num_points < 1)
    repeat = 0;
  if (repeat == 0) {
    cerr << "This program receives it's input from stdin" << endl;
    cerr << "in the following format:" << endl;
    cerr << "1 = balls, 0=points" << endl;
    cerr << "Number of dimensions of balls or points (>= 2)" << endl;
    cerr << "Number of points/balls (>= 1)" << endl;
    cerr << "Next come the points/balls with the center first," << endl;
    cerr << "and then the radius if they are balls" << endl;
    exit(1);
  }
  centers = new float *[num_points];
  if (doball & 1)
    radii = new float[num_points];
  else
    radii = NULL;
  for (ii = 0; ii < num_points; ii++) {
    centers[ii] = new float[num_dim];
    for (jj = 0; jj < num_dim; jj++)
      cin >> centers[ii][jj];
    if (doball)
      cin >> radii[ii];
  }
  out_center = new float[num_dim];
  bound_arr = new ulong[num_dim];
  ulong work_size = (1 << 15);
  void *work = (void *)(new unsigned char[work_size]);

  float radius;
  cout << "Read " << num_points << " " << num_dim << "D "
       << (doball ? "balls" : "points") << endl;
  ulong in_bound_size = 0;
  ulong out_bound_size;
  for (ii = 0; ii < repeat; ii++) {
    out_bound_size = num_dim;
    EnclosingBall(num_dim, num_points, centers, radii, out_center, radius, NULL,
                  bound_arr, in_bound_size, &out_bound_size, work, work_size);
    // If requested, use old bounding information for next calculation
    if (doball & 2)
      in_bound_size = out_bound_size;
  }

  cout << "Radius is " << radius << endl;
  cout << "Center is (";
  for (jj = 0; jj < num_dim; jj++) {
    cout << out_center[jj];
    if (jj != num_dim - 1)
      cout << " ";
    else
      cout << ")" << endl;
  }
  delete[] bound_arr;
  delete[] ((unsigned char *)work);
  delete[] out_center;
  for (ii = 0; ii < num_points; ii++)
    delete[] centers[ii];
  delete[] centers;
  return 0;
}
