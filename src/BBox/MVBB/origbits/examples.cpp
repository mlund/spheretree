void compute_mvbb(const ArrPoint3d &arr) {
  Timer tm;
  gdiam_real *p_arr, *p_start;
  GBBox ap_bbox;
  // double  num;

  tm.start();

  p_arr = (gdiam_real *)malloc(sizeof(gdiam_real) * arr.size() * 3);
  p_start = p_arr;
  for (int ind = 0; ind < arr.size(); ind++) {
    p_arr[0] = arr[ind][0];
    p_arr[1] = arr[ind][1];
    p_arr[2] = arr[ind][2];
    p_arr += 3;
  }

  gdiam_bbox bb, bb1;
  gdiam_point *ptr_arr;

  Timer tm_reg;

  tm_reg.start();

  ptr_arr = gdiam_convert(p_start, arr.size());
  bb = gdiam_approx_const_mvbb(ptr_arr, arr.size(), 0, &ap_bbox);

  tm_reg.end();

  Timer tm_reg2;

  tm_reg2.start();

  // bb1 = gdiam_approx_mvbb_grid( ptr_arr, arr.size(), 5 );
  bb1 = gdiam_approx_mvbb_grid_sample(ptr_arr, arr.size(), 5, 400);

  tm_reg2.end();

  free(ptr_arr);
  free(p_start);

  tm.end();

  printf("Overall time: %g\n", tm.seconds());
  printf("Time for const approx   : %g\n", tm_reg.seconds());

  printf("Volume on axis para bbox  : %g\n", ap_bbox.volume());

  printf("Volume of const approx    : %g\n", bb.volume());
  printf("Volume of grid            : %g\n", bb1.volume());
  printf("\n\n\n\n");
  fflush(stdout);
}

void test_itself(gdiam_real *points, int num) {
  GPointPair pair;

  printf("Computing the diameter for %d points selected "
         "uniformly from the unit cube\n",
         num);
  pair = gdiam_approx_diam_pair((gdiam_real *)points, num, 0.0);
  printf("Diameter distance: %g\n", pair.distance);
  printf("Points realizing the diameter\n"
         "\t(%g, %g, %g) - (%g, %g, %g)\n",
         pair.p[0], pair.p[1], pair.p[2], pair.q[0], pair.q[1], pair.q[2]);

  gdiam_point *pnt_arr;
  gdiam_bbox bb;

  pnt_arr = gdiam_convert((gdiam_real *)points, num);

  printf("Computing a tight-fitting bounding box of the point-set\n");
  bb = gdiam_approx_mvbb_grid_sample(pnt_arr, num, 5, 400);

  printf("Resulting bounding box:\n");
  bb.dump();

  printf("Axis parallel bounding box\n");
  GBBox bbx;
  bbx.init();
  for (int ind = 0; ind < num; ind++)
    bbx.bound(points + (ind * 3));
  bbx.dump();
}
