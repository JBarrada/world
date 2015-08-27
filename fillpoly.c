int  nodes, nodeX[MAX_POLY_CORNERS], pixelX, pixelY, i, j, swap;

//  Loop through the rows of the image.
for (pixelY=IMAGE_TOP; pixelY<IMAGE_BOT; pixelY++) {

	//  Build a list of nodes.
	nodes=0; 
	j=polyCorners-1;
	for (i=0; i<polyCorners; i++) {
		if (polyY[i]<(double) pixelY && polyY[j]>=(double) pixelY ||  polyY[j]<(double) pixelY && polyY[i]>=(double) pixelY) {
			nodeX[nodes++] = (int) (polyX[i] + (pixelY-polyY[i]) / (polyY[j]-polyY[i]) * (polyX[j]-polyX[i])); 
		}
		j=i; 
	}

	//  Sort the nodes, via a simple “Bubble” sort.
	i=0;
	while (i<nodes-1) {
		if (nodeX[i] > nodeX[i+1]) {
			swap = nodeX[i]; 
			nodeX[i] = nodeX[i+1]; 
			nodeX[i+1] = swap; 
			if (i) 
				i--; 
		}
		else {
			i++; 
		}
	}

	//  Fill the pixels between node pairs.
	for (i=0; i<nodes; i+=2) {
		
		if   (nodeX[i  ]>=IMAGE_RIGHT) break;
		
		if   (nodeX[i+1]> IMAGE_LEFT ) {
			if (nodeX[i  ]< IMAGE_LEFT ) 
				nodeX[i  ]=IMAGE_LEFT ;
			
			if (nodeX[i+1]> IMAGE_RIGHT) 
				nodeX[i+1]=IMAGE_RIGHT;
			
			for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++) 
				fillPixel(pixelX,pixelY); 
		}
	}
}


void draw_poly_filled(point points[], int num_points) {
	int poly_top, poly_bot, poly_left, poly_right, swap, p_x, p_y, node_x[num_points], i, j, nodes;
	for (i=0; i<num_points; i++) {
		if (!i) {
			poly_top = poly_bot = points[i].y;
			poly_left = poly_right = points[i].x;
		}
		else {
			if (points[i].y > poly_top)
				poly_top = points[i].y;
			if (points[i].y < poly_bot)
				poly_bot = points[i].y;
			if (points[i].x > poly_right)
				poly_right = points[i].x;
			if (points[i].x < poly_left)
				poly_left = points[i].x;
		}
	}
	
	// loop through rows of the image
	for (p_y=poly_bot; p_y<poly_top; p_y++) {
		
		// build a list of nodes
		nodes = 0;
		j = num_points - 1;
		for (i=0; i<num_points; i++) {
			if (points[i].y<(double) p_y && points[j].y>=(double) p_y ||  points[j].y<(double) p_y && points[i].y>=(double) p_y) {
				node_x[nodes++] = (int) (points[i].x + (p_y-points[i].y) / (points[j].y-points[i].y) * (points[j].x-points[i].x)); 
			}
			j=i; 
		}
		
		// sort the nodes, via a simple “bubble” sort
		i=0;
		while (i<nodes-1) {
			if (node_x[i] > node_x[i+1]) {
				swap = node_x[i]; 
				node_x[i] = node_x[i+1]; 
				node_x[i+1] = swap; 
				if (i) 
					i--; 
			}
			else {
				i++; 
			}
		}
		
		//  Fill the pixels between node pairs.
		for (i=0; i<nodes; i+=2) {
			
			if (node_x[i] >= poly_right) 
				break;
			
			if (node_x[i+1] > poly_left) {
				if (node_x[i] < poly_left) 
					node_x[i] = poly_left;
				
				if (node_x[i+1] > poly_right) 
					node_x[i+1] = poly_right;
				
				for (p_x=node_x[i]; p_x<node_x[i+1]; p_x++) 
					fillPixel(p_x,p_y);
			}
		}
	}
}