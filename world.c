typedef struct world {
	int resolution;
	
	double sealevel;
	double below_sealevel_p;
	
	int num_oceans;
	double ocean_size_p;
	double ocean_depth_p;
	
	int num_peaks; 
	double peak_height_p; 
	double peak_size_p; 
	
	int num_valleys; 
	double valley_height_p;
	double valley_size_p;
	
	double elev_delta;
	int smooth_order;
	
	double elevations[800];
} world;

void generate_world(world* w) {
	int peak_max = (double)(w->elev_delta*w->peak_height_p);
	int valley_max = (double)(w->elev_delta*w->valley_height_p);
	double unit_node_dist = 2*sin(M_PI/w->resolution);

	int i, j;
	
	// create oceans
	int ocean_locs[w->num_oceans];
	int ocean_range[w->num_oceans];
	for (i=0; i<w->num_oceans; i++) {
		int ocean_pos;
		int on_ocean = 1;
		while (on_ocean) {
			ocean_pos = rand() % w->resolution;
			on_ocean = 0;
			for (j=0; j<i; j++)
				if (abs(ocean_pos-ocean_range[j]) < ocean_range[j])
					on_ocean = 1;
		}
		ocean_locs[i] = ocean_pos;
		ocean_range[i] = (rand() % (int)(w->resolution*w->ocean_size_p) + (w->resolution*w->ocean_size_p)) / 4;
	}
	
	// find peaks
	int peak_locs[w->num_peaks];
	double peak_elev[w->num_peaks];
	double peak_range[w->num_peaks];
	for (i=0; i<w->num_peaks; i++) {
		int peak_pos;
		int on_peak = 1;
		while (on_peak) {
			peak_pos = rand() % w->resolution;
			on_peak = 0;
			for (j=0; j<i; j++)
				if (abs(peak_pos-peak_locs[j]) < peak_range[j])
					on_peak = 1;
		}
		peak_locs[i] = peak_pos;
		peak_elev[i] = ((rand()%peak_max)+peak_max)/2;
		peak_max -= (1/(w->num_peaks*2))*peak_max;
		peak_range[i] = (rand() % (int)(w->resolution*w->peak_size_p) + (w->resolution*w->peak_size_p)) / 4;
	}
	
	// find valleys
	int valley_locs[w->num_valleys];
	double valley_elev[w->num_valleys];
	double valley_range[w->num_valleys];
	for (i=0; i<w->num_valleys; i++) {
		int valley_pos;
		int on_peak = 1;
		while (on_peak) {
			valley_pos = rand() % w->resolution;
			on_peak = 0;
			for (j=0; j<w->num_peaks; j++)
				if (abs(valley_pos-peak_locs[j]) < peak_range[j])
					on_peak = 1;
		}
		valley_locs[i] = valley_pos;
		valley_elev[i] = ((rand()%valley_max)+valley_max)/2;
		valley_range[i] = (rand() % (int)(w->resolution*w->valley_size_p) + (w->resolution*w->valley_size_p)) / 4;
	}
	
	// create features
	for (i=0; i<w->resolution; i++) {
		double elevation = w->sealevel;  // nominal
		
		// see if influenced by ocean
		for (j=0; j<w->num_oceans; j++) {
			int dist_to_j = abs(i-ocean_locs[j]);
			dist_to_j = (dist_to_j<w->resolution-dist_to_j)?dist_to_j:w->resolution-dist_to_j;
			if (dist_to_j < ocean_range[j]) {
				elevation -= w->elev_delta*w->ocean_depth_p;
			}
		}
		
		// see if influenced by peak
		double peak_influence=0, pi_temp;
		for (j=0; j<w->num_peaks; j++) {
			int dist_to_j = abs(i-peak_locs[j]);
			dist_to_j = (dist_to_j<w->resolution-dist_to_j)?dist_to_j:w->resolution-dist_to_j;
			if (dist_to_j < peak_range[j]) {
				pi_temp = peak_elev[j] - (peak_elev[j]/peak_range[j])*dist_to_j;
				peak_influence = (pi_temp>peak_influence)?pi_temp:peak_influence;
			}
		}
		
		// see if influenced by valley
		double valley_influence=0, vi_temp;
		for (j=0; j<w->num_valleys; j++) {
			int dist_to_j = abs(i-valley_locs[j]);
			dist_to_j = (dist_to_j<w->resolution-dist_to_j)?dist_to_j:w->resolution-dist_to_j;
			if (dist_to_j < valley_range[j]) {
				vi_temp = valley_elev[j] - (valley_elev[j]/valley_range[j])*dist_to_j;
				valley_influence = (vi_temp>valley_influence)?vi_temp:valley_influence;
			}
		}
		
		elevation += peak_influence;
		elevation -= valley_influence;
		w->elevations[i] = elevation;
	}
	
	// smoothing
	double e_delta=0, step=0, influence=0, total=0;
	for (i=1; i<=w->smooth_order; i++)
		total += i*i;
	for (j=w->smooth_order; j>0; j--){
		int order = j*j;
		influence = order/total;
		
		for (i=0; i<w->resolution; i++) {
			if (i%order == 0) {
				double v = w->elev_delta;
				double next_delta = ((rand()%(int)v)-(v*w->below_sealevel_p))- e_delta;
				next_delta *= influence;
				step = (next_delta)/order;
			}
			w->elevations[i] += e_delta;
			e_delta += step;
		}
	}
	
	for (i=0; i<w->resolution; i++) {
		w->elevations[i] -= w->elev_delta*0.8;
	}
}

void draw_world_circular(world* w, int cx, int cy, double scale, double longitude) {
	longitude += M_PI/2.0;
	point points[w->resolution];
	int i;
	double x, y, x_, y_, radius, step=(2*M_PI)/w->resolution;

	for (i=0; i<w->resolution; i++) {
		radius = w->elevations[w->resolution-i-1] * scale;
		x = radius * cos(step*i);
		y = radius * sin(step*i);
		
		x_ = x*cos(longitude) - y*sin(longitude);
		y_ = y*cos(longitude) + x*sin(longitude);
		
		points[i] = (point) {x_+cx, y_+cy};
	}
	
	circle_filled(cx, cy, w->sealevel * scale, 0x03);
	poly_filled(points, w->resolution, 0xff);
}

void draw_world_flat(world* w, int window_w, double scale, double longitude) {
	while (longitude >= 2*M_PI)
		longitude -= 2*M_PI;
	
	double circumference = ((2*M_PI)*w->sealevel);
	double step = circumference/w->resolution;
	double points_in_view = window_w/(step*scale) + 4.0;
	
	double c_index = (longitude/(2*M_PI)) * w->resolution;
	
	int start_point = floor(c_index - (points_in_view/2.0));
	int num_points = ceil(points_in_view);
	
	double start_offset = (window_w/2.0) - scale*step*(c_index-start_point);
	
	point points[num_points + 2];
	
	double x, y, elevation, cy = -500;
	
	
	//printf("CI: %f  PIV: %f  SP: %d  SO: %f\n", c_index, points_in_view, start_point, start_offset);
	
	
	int i, i_, p=0;
	for (i=start_point; i<(start_point+num_points); i++) {
		i_ = i;
		if (i < 0)
			i_ = i + w->resolution;
		if (i >= w->resolution)
			i_ = i - w->resolution;
		
		//printf("I: %d  I_: %d  res: %d\n",i, i_, w->resolution);
		
		elevation = w->elevations[i_] * scale;
		x = p * step * scale;
		y = elevation;
		
		//printf("I_: %d  ELEV: %f  X: %f  Y: %f  P: %d\n", i_, elevation, x, y, p);
		
		points[p] = (point) {x+start_offset, y+cy};
		p++;
		
		//getch();
	}
	//getch();
	
	
	points[p+1] = (point) {start_offset, cy};
	points[p] = (point) {start_offset+num_points*step*scale, cy};
	
	point sealevel[4];
	sealevel[0] = (point) {0, cy};
	sealevel[1] = (point) {0, cy+w->sealevel*scale};
	sealevel[2] = (point) {0+window_w, cy+w->sealevel*scale};
	sealevel[3] = (point) {0+window_w, cy};
	
	poly_filled(sealevel, 4, 0x03);
	poly_filled(points, num_points + 2, 0xff);
	
	
	
	// int i;
	// double cx, cy, x, y, x_, y_, elevation, circumference=((2*M_PI)*w->sealevel), step=circumference/w->resolution;
	
	// cx = -(longitude/(2*M_PI)) * (circumference*scale) + window_w/2;
	// cy = -500;
	
	// for (i=0; i<w->resolution; i++) {
		// elevation = w->elevations[i] * scale;
		// x = i * step * scale;
		// y = elevation;
		
		// points[i] = (point) {x+cx, y+cy};
	// }		
	// points[i+1] = (point) {cx, cy};
	// points[i] = (point) {cx+circumference*scale, cy};

	// point sealevel[4];
	// sealevel[0] = (point) {cx, cy};
	// sealevel[1] = (point) {cx, cy+w->sealevel*scale};
	// sealevel[2] = (point) {cx+circumference*scale, cy+w->sealevel*scale};
	// sealevel[3] = (point) {cx+circumference*scale, cy};
	
	// poly_filled(sealevel, 4, 0x03);
	// poly_filled(points, w->resolution + 2, 0xff);
	
}
