#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "gfx.c"
#include "world.c"

#define NUM_POINTS 800
#define SEALEVEL 200

world w;

double longitude = 0;
double longitude_vel = 0;
double longitude_decay = 0.99;

double sun_longitude = 0;
int TOD = 0;

void draw() {
	switch (TOD) {
		case 0: night(); break;
		case 1: sunrise(); break;
		case 2: day(); break;
		case 3: sunset(); break;
	}
	
	world_time(sun_longitude, longitude, (2*M_PI)*w.sealevel, 3);
	
	draw_world_flat(&w, 640, 3, longitude);
	draw_world_circular(&w, 120, 360, 0.5, longitude);
}

void idle() {
	sun_longitude += 0.00002;
	if (sun_longitude >= 2*M_PI)
		sun_longitude -= 2*M_PI;
	
	longitude += longitude_vel;
	longitude_vel *= longitude_decay;
	render();
	usleep(1000000/30.0);
}

void refresh() {
	w.resolution = NUM_POINTS;
	
	w.sealevel = SEALEVEL;
	w.below_sealevel_p = 0.1;
	
	w.num_oceans = 1;
	w.ocean_size_p = 0.3;
	w.ocean_depth_p = 0.8;
	
	w.num_peaks = 3;
	w.peak_height_p = 4.5; 
	w.peak_size_p =  0.1;
	
	w.num_valleys = 3;
	w.valley_height_p = 1.0;
	w.valley_size_p = 0.05;
	
	w.elev_delta = 10;
	w.smooth_order = 3;
	
	generate_world(&w);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'z': refresh(); break;
		case 100: longitude_vel += 0.001; break;
		case 97: longitude_vel -= 0.001; break;
		case 49: TOD = 0; break;
		case 50: TOD = 1; break;
		case 51: TOD = 2; break;
		case 52: TOD = 3; break;
	}
	// printf("K: %d\n", key);
}

void main() {
	srand(time(NULL));
	refresh();
	
	init(idle, draw, keyboard);
}