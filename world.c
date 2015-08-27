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
	
	int elev_delta;
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
				double v = w->elev_delta*influence;
				double next_delta = ((rand()%(int)v)-(v*w->below_sealevel_p))- e_delta;
				step = (next_delta)/order;
			}
			w->elevations[i] += e_delta;
			e_delta += step;
		}
	}
}
	