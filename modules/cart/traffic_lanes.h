/* the points the traffic cars drive through, two lanes for each of Tails's
 * and Rouge's routes. a car takes a point a frame. under MISSION_HD the cars
 * drive the _hd_ lanes instead, a point every two frames */
#ifndef CART_TRAFFIC_LANES_H
#define CART_TRAFFIC_LANES_H
#include <ninja.h>

extern NJS_POINT3 cart_tails_lane_0[12234], cart_tails_lane_1[10199];
extern NJS_POINT3 cart_rouge_lane_0[13209], cart_rouge_lane_1[12900];
extern NJS_POINT3 cart_tails_hd_lane_0[9201], cart_tails_hd_lane_1[8757];
extern NJS_POINT3 cart_rouge_hd_lane_0[10764], cart_rouge_hd_lane_1[10700];

#endif
