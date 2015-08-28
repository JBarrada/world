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

void draw() {
	draw_world_circular(&w, 120, 360, 0.5, longitude);
	draw_world_flat(&w, 640, 3, longitude);
	
	line(320, 0, 320, 480, 0xe0);
	line(120, 360, 120, 480, 0xe0);
}

void idle() {
	longitude += 0.0001;
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
	
	//longitude = 0;
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