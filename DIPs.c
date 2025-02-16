//file name : dips.c
//
#include "DIPs.h"
#include "Constants.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
/* reverse image color */
void Negative(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
	int x, y;
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			R[x][y] = MAX_PIXEL - R[x][y];
			G[x][y] = MAX_PIXEL - G[x][y];
			B[x][y] = MAX_PIXEL - B[x][y];
		}
	}
}

/* color filter */
void ColorFilter (unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT],
		int target_r, int target_g, int target_b, int threshold, int replace_r, int replace_g, int replace_b) {

	int x, y;

	replace_r = (replace_r > MAX_PIXEL) ? MAX_PIXEL : (replace_r < 0) ? 0 : replace_r;
	replace_g = (replace_g > MAX_PIXEL) ? MAX_PIXEL : (replace_g < 0) ? 0 : replace_g;
	replace_b = (replace_b > MAX_PIXEL) ? MAX_PIXEL : (replace_b < 0) ? 0 : replace_b;

	for (y = 0; y < HEIGHT; y++){
		for (x = 0; x < WIDTH; x++){
			if (abs(R[x][y] - target_r) <= threshold
					&& abs(G[x][y] - target_g) <= threshold
					&& abs(B[x][y] - target_b) <= threshold) {
				R[x][y] = replace_r;
				G[x][y] = replace_g;
				B[x][y] = replace_b;
			}
		}
	}
}

/* edge detection */
void Edge(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
	int             x, y, m, n, a, b;
	unsigned char   tmpR[WIDTH][HEIGHT];
	unsigned char   tmpG[WIDTH][HEIGHT];
	unsigned char   tmpB[WIDTH][HEIGHT];
	for (y = 0; y < HEIGHT; y++){
		for (x = 0; x < WIDTH; x++) {
			tmpR[x][y] = R[x][y];
			tmpG[x][y] = G[x][y];
			tmpB[x][y] = B[x][y];
		}
	}
	int sumR = 0;   /* sum of the intensity differences with neighbors */
	int sumG = 0;
	int sumB = 0;
	for (y = 1; y < HEIGHT - 1; y++){
		for (x = 1; x < WIDTH - 1; x++){
			for (n = -1; n <= 1; n++){
				for (m = -1; m <= 1; m++) {
					a = (x + m >= WIDTH) ? WIDTH - 1 : (x + m < 0) ? 0 : x + m;
					b = (y + n >= HEIGHT) ? HEIGHT - 1 : (y + n < 0) ? 0 : y + n;
					sumR += (tmpR[x][y] - tmpR[a][b]);
					sumG += (tmpG[x][y] - tmpG[a][b]);
					sumB += (tmpB[x][y] - tmpB[a][b]);
				}
			}
			R[x][y] = (sumR > MAX_PIXEL) ? MAX_PIXEL: (sumR < 0) ? 0: sumR;
			G[x][y] = (sumG > MAX_PIXEL) ? MAX_PIXEL: (sumG < 0) ? 0: sumG;
			B[x][y] = (sumB > MAX_PIXEL) ? MAX_PIXEL: (sumB < 0) ? 0: sumB;
			sumR = sumG = sumB = 0;
		}
	}
	/* set all 24 borders to 0 */
	for (y = 0; y < HEIGHT; y++) {
		R[0][y] = 0;
		G[0][y] = 0;
		B[0][y] = 0;
		R[WIDTH - 1][y] = 0;
		G[WIDTH - 1][y] = 0;
		B[WIDTH - 1][y] = 0;
	}
	for (x = 0; x < WIDTH; x++) {
		R[x][0] = 0;
		G[x][0] = 0;
		B[x][0] = 0;
		R[x][HEIGHT - 1] = 0;
		G[x][HEIGHT - 1] = 0;
		B[x][HEIGHT - 1] = 0;
	}
}

/* mirror image horizontally */
void HMirror(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
	int             x, y;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH / 2; x++) {
			R[WIDTH - 1 - x][y] = R[x][y] ;
			G[WIDTH - 1 - x][y] = G[x][y] ;
			B[WIDTH - 1 - x][y] = B[x][y] ;
		}
	}
}
void Pixelate(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT], int block_size)
{
    int x, y, bx, by;
    int sumR, sumG, sumB;
    int count;

    for (y = 0; y < HEIGHT - HEIGHT % block_size; y += block_size) {
        for (x = 0; x < WIDTH - WIDTH % block_size; x += block_size) {
            sumR = sumG = sumB = 0; // initialize
            count = 0;

            for (by = 0; by < block_size; by++) {
                for (bx = 0; bx < block_size; bx++) {
                    sumR += R[x + bx][y + by];
                    sumG += G[x + bx][y + by];
                    sumB += B[x + bx][y + by];
                    count++;
                }
            }

            // calculate average RGB value
            sumR = sumR / count;
            sumG = sumG / count;
            sumB = sumB / count;

            // write new values to pixels in the block
            for (by = 0; by < block_size; by++) {
                for (bx = 0; bx < block_size; bx++) {
                    R[x + bx][y + by] = sumR;
                    G[x + bx][y + by] = sumG;
                    B[x + bx][y + by] = sumB;
                }
            }
        }
    }
}
/* add border to the image */
void AddBorder(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT],
		char color[SLEN], int border_width) {
	int x, y;
	int border_r = 255;
	int border_g = 255;
	int border_b = 255;
	if (!strcmp(color, "black")) {
		border_r = 0;
		border_g = 0;
		border_b = 0;
	} else if (!strcmp(color, "white")) {
		border_r = 255;
		border_g = 255;
		border_b = 255;
	} else if (!strcmp(color, "red")) {
		border_r = 255;
		border_g = 0;
		border_b = 0;
	} else if (!strcmp(color, "green")) {
		border_r = 0;
		border_g = 255;
		border_b = 0;
	} else if (!strcmp(color, "blue")) {
		border_r = 0;
		border_g = 0;
		border_b = 255;
	} else if (!strcmp(color, "yellow")) {
		border_r = 255;
		border_g = 255;
		border_b = 0;
	} else if (!strcmp(color, "cyan")) {
		border_r = 0;
		border_g = 255;
		border_b = 255;
	} else if (!strcmp(color, "pink")) {
		border_r = 255;
		border_g = 192;
		border_b = 203;
	} else if (!strcmp(color, "orange")) {
		border_r = 255;
		border_g = 165;
		border_b = 0;
	} else {
		printf("Unsupported color.\n");
		return;
	}
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			if ((y < border_width) || (y > HEIGHT - 1 - border_width) ||
					(x < border_width) || (x > WIDTH - 1 - border_width)) {
				R[x][y] = border_r;
				G[x][y] = border_g;
				B[x][y] = border_b;
			}
		}
	}
}

/* change color image to black & white */
void BlackNWhite(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
	int             x, y, tmp;

	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			tmp = (R[x][y] + G[x][y] + B[x][y]) / 3;
			R[x][y] = G[x][y] = B[x][y] = tmp;
		}
	}
}

/* flip image vertically */
void VFlip(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
	int             x, y;
	unsigned char   r, g, b;

	for (y = 0; y < HEIGHT/2; y++) {
		for (x = 0; x < WIDTH; x++) {
			r = R[x][HEIGHT - 1 - y];
			g = G[x][HEIGHT - 1 - y];
			b = B[x][HEIGHT - 1 - y];

			R[x][HEIGHT - 1 - y] = R[x][y];
			G[x][HEIGHT - 1 - y] = G[x][y];
			B[x][HEIGHT - 1 - y] = B[x][y];

			R[x][y] = r;
			G[x][y] = g;
			B[x][y] = b;
		}
	}
}

/* Shuffle the image */
void Shuffle(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
	int block_width = WIDTH/SHUFF_WIDTH_DIV;
	int block_height = HEIGHT/SHUFF_HEIGHT_DIV;
	int i, j, x, y;
	unsigned char tempR[block_width][block_height];
	unsigned char tempG[block_width][block_height];
	unsigned char tempB[block_width][block_height];

	for (i = 0; i < SHUFF_HEIGHT_DIV/2; i++){ // only need to iterate throught half of the blocks
		for (j = 0; j < SHUFF_WIDTH_DIV; j++){
			for (x = 0; x < block_width; x++){
				for (y = 0; y < block_height; y++){
					tempR[x][y] = R[x + (j * block_width)][y + (i * block_height)];
					tempG[x][y] = G[x + (j * block_width)][y + (i * block_height)];
					tempB[x][y] = B[x + (j * block_width)][y + (i * block_height)];
					R[x + (j * block_width)][y + (i * block_height)] = R[x + ((SHUFF_WIDTH_DIV - 1 -j) * block_width)][y + ((SHUFF_HEIGHT_DIV - 1 -i) * block_height)];
					G[x + (j * block_width)][y + (i * block_height)] = G[x + ((SHUFF_WIDTH_DIV - 1 -j) * block_width)][y + ((SHUFF_HEIGHT_DIV - 1 -i) * block_height)];
					B[x + (j * block_width)][y + (i * block_height)] = B[x + ((SHUFF_WIDTH_DIV - 1 -j) * block_width)][y + ((SHUFF_HEIGHT_DIV - 1 -i) * block_height)];
					R[x + ((SHUFF_WIDTH_DIV - 1 -j) * block_width)][y + ((SHUFF_HEIGHT_DIV - 1 -i) * block_height)] = tempR[x][y];
					G[x + ((SHUFF_WIDTH_DIV - 1 -j) * block_width)][y + ((SHUFF_HEIGHT_DIV - 1 -i) * block_height)] = tempG[x][y];
					B[x + ((SHUFF_WIDTH_DIV - 1 -j) * block_width)][y + ((SHUFF_HEIGHT_DIV - 1 -i) * block_height)] = tempB[x][y];
				}
			}
		}
	}

}