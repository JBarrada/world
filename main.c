#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "gfx.c"
#include "world.c"

#define NUM_POINTS 800
#define SEALEVEL 200
point points[NUM_POINTS];

void rotate(int cx, int cy, double radians) {
	int i;
	double x, y, x_, y_;
	for (i=0; i<NUM_POINTS; i++) {
		x = points[i].x - cx;
		y = points[i].y - cy;
		x_ = x*cos(radians) - y*sin(radians);
		y_ = y*cos(radians) + x*sin(radians);
		
		points[i] = (point) {cx+x_, cy+y_};
	}
}

void draw() {
	circle_filled(320, -240, SEALEVEL * 2, 0x03);
	poly_filled(points, NUM_POINTS, 0xff);
}

void idle() {
	rotate(320, -240, 0.002);
	render();
	usleep(1000000/30.0);
}

void refresh() {
	world w;
	w.resolution = NUM_POINTS;
	
	w.sealevel = SEALEVEL;
	w.below_sealevel_p = 0.1;
	
	w.num_oceans = 1;
	w.ocean_size_p = 0.3;
	w.ocean_depth_p = 0.8;
	
	w.num_peaks =  3;
	w.peak_height_p = 4.5; 
	w.peak_size_p =  0.1;
	
	w.num_valleys = 3;
	w.valley_height_p = 1.0;
	w.valley_size_p = 0.05;
	
	w.elev_delta = 10;
	w.smooth_order = 3;
	
	generate_world(&w);
	
	int i;
	double x, y, radius, step=(2*M_PI)/NUM_POINTS, cx=320, cy=-240;

	for (i=0; i<NUM_POINTS; i++) {
		radius = w.elevations[i] * 2;
		x = radius * cos(step*i);
		y = radius * sin(step*i);
		points[i] = (point) {x+cx, y+cy};
	}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'z': refresh(); break;
	}
}

void main() {
	srand(time(NULL));
	refresh();
	
	init(idle, draw, keyboard);
}