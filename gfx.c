#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glext.h>

unsigned char screen_buffer[WIDTH_W*HEIGHT_W];

void (*draw_function)();

unsigned char get_byte_color(int hexcolor) {
	int red = ((hexcolor >> 16) & 0xff) / 32;
	int green = ((hexcolor >> 8) & 0xff) / 32;
	int blue = (hexcolor & 0xff) / 64;

	return (red << 5) | (green << 2) | blue;
}


void set_pixel(int x, int y, unsigned char c) {
	if (x<WIDTH_W & y<HEIGHT_W & x>=0 & y>=0) {
		screen_buffer[y*WIDTH_W+x] = c;
	}	
}

void line(int x1,int y1,int x2,int y2, unsigned char c) {
	int dx, dy, dxabs, dyabs, x, y, px, py;
	dx = (x2-x1);
	dy = (y2-y1);
	dxabs=abs(dx);
	dyabs=abs(dy);
	
	if (dyabs < 1000 & dxabs < 1000) {
		x=dyabs>>1;
		y=dxabs>>1;
		px=x1;
		py=y1;
		
		int i;
		if (dxabs>=dyabs) {
			for(i=0;i<dxabs;i++) {
				y+=dyabs;
				if (y>=dxabs) {
					y-=dxabs;
					py+=(dy>=0)?1:-1;
				}
				px+=(dx>=0)?1:-1;
				set_pixel(px,py,c);
			}
		}
		else {
			for(i=0;i<dyabs;i++) {
				x+=dxabs;
				if (x>=dyabs) {
					x-=dyabs;
					px+=(dx>=0)?1:-1;
				}
				py+=(dy>=0)?1:-1;
				set_pixel(px,py,c);
			}
		}
	}
}

void circle(int cx, int cy, double r, unsigned char c) {
	int i, res=10;
	double x, y, x_, y_;
	for (i=0; i<=res; i++) {
		x = r * cos(((M_PI/2)/res)*i);
		y = r * sin(((M_PI/2)/res)*i);
		
		if (i != 0) {
			line(cx+x_, cy+y_, cx+x, cy+y, c);
			line(cx-x_, cy+y_, cx-x, cy+y, c);
			line(cx+x_, cy-y_, cx+x, cy-y, c);
			line(cx-x_, cy-y_, cx-x, cy-y, c);
		}
		
		x_ = x;
		y_ = y;
	}
}

void circle_filled(int cx, int cy, int r, unsigned char c) {
	int i, tx, ty;
	int r2 = r * r;
	int area = r2 << 2;
	int rr = r << 1;

	for (i=0; i<area; i++)
	{
		tx = (i % rr) - r;
		ty = (i / rr) - r;

		if (tx * tx + ty * ty <= r2)
			set_pixel(cx+tx, cy+ty, c);
	}
}

void poly_filled(point points[], int num_points, unsigned char c) {
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
					set_pixel(p_x, p_y, c);
			}
		}
	}
}

void rectangle_filled(int x, int y, int width, int height, unsigned char c) {
	point rect[4];
	rect[0] = (point) {x, y};
	rect[1] = (point) {x+width, y};
	rect[2] = (point) {x+width, y+height};
	rect[3] = (point) {x, y+height};
	poly_filled(rect, 4, c);
}

void v_gradient(y1, y2, c1, c2) {
	int steps = y2-y1;
	double r1, r2, g1, g2, b1, b2, rd, gd, bd;
	r1 = ((c1 >> 16) & 0xff);
	g1 = ((c1 >> 8) & 0xff);
	b1 = (c1 & 0xff);
	
	r2 = ((c2 >> 16) & 0xff);
	g2 = ((c2 >> 8) & 0xff);
	b2 = (c2 & 0xff);
	
	rd = (r2-r1)/steps;
	gd = (g2-g1)/steps;
	bd = (b2-b1)/steps;
	
	int y;
	for (y=y1; y<y2; y++) {
		line(0, y, WIDTH_W, y, get_byte_color(((int)r1<<16) | ((int)g1<<8) | (int)b1));
		r1 += rd;
		g1 += gd;
		b1 += bd;
	}
}

void render() {
	memset(screen_buffer, 0, WIDTH_W*HEIGHT_W);
	
	draw_function();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(WIDTH_W, HEIGHT_W, GL_RGB, GL_UNSIGNED_BYTE_3_3_2, screen_buffer);
	glutSwapBuffers();
}

void init(void (*idle)(), void (*draw)(), void (*keyboard)(unsigned char key, int x, int y)) {
	draw_function = draw;
	
	char fakeParam[] = "";
	char *fakeargv[] = { fakeParam, NULL };
	int fakeargc = 1;
	
	glutInit(&fakeargc, fakeargv);
	glutInitWindowSize(WIDTH_W, HEIGHT_W);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("!");
	glutIdleFunc(idle);
	glutDisplayFunc(render);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
}

