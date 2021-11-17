#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_ROW 24
#define MAX_COL 80

#define MAX_VERTS 1024
#define MAX_TRIS 256

struct vertex
{
	int x, y;
};

struct triangle
{
	// index of vertex in vert listing
	int verts[3];
	char c;
};

void usage(char *as)
{
	fprintf(stderr, "Usage: %s filename\n", as);
	fprintf(stderr, "\tfilename\tInput file in format described below\n");

	fprintf(stderr,
			"\nThe input file is a CSV listing of vertices and triangles.\n"
			"Vertices are listed first, one per line. Each line should start\n"
			"with a 'v', followed by two integers, the X,Y coordinates.\n"
			"After that triangles can be defined by lines starting with 't'\n"
			"followed by three integers, the index of each of the three\n"
			"vertices in the order they are listed above, and a single\n"
			"character, which will be the fill of the triangle.\n"
		);

	fprintf(stderr,
			"\nThe below example draws a single triangle with\nvertices "
			"3,20; 20,20; and 15,50:\n\n"
			"v,3,20\n"
			"v,20,20\n"
			"v,12,50\n"
			"t,0,1,2,a\n"
		);
}

struct vertex parse_vertex(char *in)
{
	struct vertex vert;
	char *field;

	field = in;

	field = strstr(field, ",") + 1;
	vert.x = atoi(field);

	field = strstr(field, ",") + 1;
	vert.y = atoi(field);

	return vert;
}

struct triangle parse_tris(char *in)
{
	struct triangle tri;
	char *field;

	field = in;

	field = strstr(field, ",") + 1;
	tri.verts[0]  = atoi(field);

	field = strstr(field, ",") + 1;
	tri.verts[1]  = atoi(field);

	field = strstr(field, ",") + 1;
	tri.verts[2]  = atoi(field);

	field = strstr(field, ",") + 1;
	tri.c = *(field);

	return tri;
}

// for Point P at (row,col) and the line between v1,v2:
// if P is on the line, return 0
// if P is left of the line, return -1
// if P is right of the line return 1
int edge(struct vertex v1, struct vertex v2, int row, int col)

{
	// This comes from the line formula (y=mx+b)
	// m = (v2.y - v1.y)/(v2.x-v1.x)
	// x = col - v1.x
	// y = row - v1.y
	// b = 0
	// rearranging this we get:
	int e = ((col - v1.x) * (v2.y - v1.y)) - ((row - v1.y) * (v2.x - v1.x));

	if (e == 0) /* on line */
		return 0;
	else if (e < 0) /* left of line */
		return -1;
	else /* right of line */
		return 1;
}

int intersect(struct vertex *verts, struct triangle tri, int row, int col)
{
	int e1, e2, e3;
	int v1, v2, v3;
	v1 = tri.verts[0];
	v2 = tri.verts[1];
	v3 = tri.verts[2];

	e1 = edge(verts[v1], verts[v2], row, col);
	e2 = edge(verts[v2], verts[v3], row, col);
	e3 = edge(verts[v3], verts[v1], row, col);

	// If on the same side of all lines, the point is in the triangle
	if (e1 == e2 && e1 == e3)
		return 1;
	
	return 0;
}

int closest_intersect(struct vertex *verts, struct triangle *tris, int tri_count, int row, int col)
{
	for (int tri = 0; tri < tri_count; ++tri)
	{
		int found = intersect(verts, tris[tri], row, col);
		if (found)
		{
			return tri;
		}
	}

	return -1;
}

void draw_triangles(struct vertex *verts, struct triangle *tris,
					int tri_count)
{
	for (int row = MAX_ROW; row > 0; --row)
	{
		for (int col = 0; col < MAX_COL; ++col)
		{
			int found = closest_intersect(verts, tris, tri_count, row, col);
			if (found >= 0)
				putchar(tris[found].c);
			else
				putchar(' ');
		}

		putchar('\n');
	}
}

int main(int argc, char *argv[])
{
	struct vertex verts[MAX_VERTS];
	struct triangle tris[MAX_TRIS];
	int vert_count, tri_count;
	char line[32];
	FILE *f;

	if (argc != 2)
	{
		usage(argv[0]);
		return -1;
	}

	f = fopen(argv[1], "r");
	if (f == NULL)
	{
		fprintf(stderr, "Invalid filename '%s'\n", argv[1]);
		return -1;
	}

	vert_count = 0;
	tri_count = 0;

	while (fgets(line, 32, f))
	{
		switch (line[0])
		{
			case 'v':
				verts[vert_count] = parse_vertex(line);
				printf("Loaded vertex %d with x=%d, y=%d\n",
					   vert_count, verts[vert_count].x, verts[vert_count].y);
				vert_count++;
				break;
			case 't':
				tris[tri_count] = parse_tris(line);
				if (tris[tri_count].verts[0] >= vert_count ||
					tris[tri_count].verts[1] >= vert_count ||
					tris[tri_count].verts[2] >= vert_count)
				{
					fprintf(stderr,
							"Triangle specified invalid vertex for line %s\n",
							line);
					return -1;
				}

				printf("Loaded triangle %d with verts[0]=%d, verts[1]=%d, verts[2]=%d, c=%c\n",
					   tri_count, tris[tri_count].verts[0], tris[tri_count].verts[1],
					   tris[tri_count].verts[2], tris[tri_count].c);
				tri_count++;
				break;
			default:
				fprintf(stderr, "Ill-formatted input for line %s\n", line);
				return -1;
		}
	}

	fclose(f);

	draw_triangles(verts, tris, tri_count);

	return 0;
}
