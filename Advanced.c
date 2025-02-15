// filename: advanced.c
#include "Advanced.h"
#include "Constants.h"
#include <string.h>
#include <time.h>
#include <math.h>
void MotionBlur(int BlurAmount, unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{

    int Ramount;
    // pixels to the right to move, if pixels and blur amount go past boundary, set to whatever wont go past boundary
    // calculate average of pixels to right
    // average = pixel values to the right / number of pixels to the right
    // example: pixel (0,0) would take take the values of each of the pixels from 0 -> ramount and then average
    // we have to find sum of value of pixels to the right by Ramount
    int RRAverage, RGAverage, RBAverage;
    int Rsum, Gsum, Bsum;
    
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {   
            Rsum = Gsum = Bsum = 0;
            //first make sure the fixed number of pixels to the right is not off bounds
            Ramount = ((x + BlurAmount) < WIDTH) ? (BlurAmount) : (WIDTH - x); 
            // then find the sum of each of the values to the right by Ramount
            for (int i = 0; i < Ramount; i++)
            {
               Rsum += R[x+i][y];
               Gsum += G[x+i][y];
               Bsum += B[x+i][y];
            }
            // whenever dividing always check divisor is not 0!!!!!!
            if (Ramount > 0)
            {
                RRAverage = Rsum / Ramount + 1;
                RGAverage = Gsum / Ramount + 1;
                RBAverage = Bsum / Ramount + 1;
            }
            else
            {
                RRAverage = RGAverage = RBAverage = 0;
            }

            R[x][y] = 0.5 * R[x][y] + 0.5 * (int)RRAverage;  
            G[x][y] = 0.5 * G[x][y] + 0.5 * (int)RGAverage;
            B[x][y] = 0.5 * B[x][y] + 0.5 * (int)RBAverage;
        }
        
    }
}
void Rotate(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT], double Angle, double ScaleFactor, int CenterX, int CenterY)
{
    // if we have
    // a b e
    // c d g
    // (x') = ae+bg 
    // (y') = ce+dg
    unsigned char Ro[WIDTH][HEIGHT], Go[WIDTH][HEIGHT], Bo[WIDTH][HEIGHT]; 
    double radianAngle = -Angle * PI / 180; // because the origin starts top left instead of bottom left, we multiple by negative angle
    double cosA = cos(radianAngle) / ScaleFactor;
    double sinA = sin(radianAngle) / ScaleFactor;
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            double x_p = ( (cosA) * (x - CenterX) + (-sinA) * (y - CenterY) ) + (CenterX); // matrix multiplication + centerx
            double y_p = ( (sinA) * (x - CenterX) + (cosA) * (y - CenterY)  ) + (CenterY);
            int x_src, y_src;
            x_src = (int)x_p;
            y_src = (int)y_p;
            
            
            if (x_src >= 0 && x_src <= WIDTH-1 && y_src >= 0 && y_src <= HEIGHT - 1) // check if the pixels are in the bounds, if not set to black
            {
                Ro[x][y] = R[x_src][y_src];
                Go[x][y] = G[x_src][y_src];
                Bo[x][y] = B[x_src][y_src];
            }
            else
            {
                Ro[x][y] = Go[x][y] = Bo[x][y] = 0;
            }
        }
    }
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            R[x][y] = Ro[x][y];
            G[x][y] = Go[x][y];
            B[x][y] = Bo[x][y];
        }
    }
}



void Posterize(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT], unsigned int rbits, unsigned int gbits, unsigned int bbits)
{    
    // posterize : take rbits = 6
    // 0000 0000 < size of char
    // ex: take pixel R[x][y] = 41
    // 0010 1001 -> take bit 6 turn to 0, turn 1-5 into 1
    // 0001 1111
    // to turn 1-5 into 1
    // create a mask from 1 -> (rbits - 1) of all 1's
    // to turn bit 6 into 0
    unsigned int rmask, rmsb, gmask, gmsb, bmask, bmsb;
    rmask = pow(2, rbits) - 1; // calculate the mask to OR everything ex: rbit = 6, we will want everything up until 2^6 to be 1 now
    rmsb = pow(2, rbits - 1); // calculate the msb in order to delete it
    
    gmask = pow(2, gbits) - 1;
    gmsb = pow(2, gbits - 1);
    
    bmask = pow(2, bbits) - 1;
    bmsb = pow(2, bbits - 1);
    
    
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            unsigned int R_new = R[x][y] | rmask; //bit wise OR
            unsigned int G_new = G[x][y] | gmask; //bit wise OR
            unsigned int B_new = B[x][y] | bmask; //bit wise OR WITH MASK TO TURN EVERYTHING THAT MATCHES MASK INTO 1
            R_new = R_new - rmsb; // remove msb (rbit)
            G_new = G_new - gmsb;
            B_new = B_new - bmsb;
            
            R[x][y] = R_new;
            G[x][y] = G_new;
            B[x][y] = B_new;
        }
    }
    
}
void FishEye(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT], double distortion_factor, double scaling_factor, double k)
{
    unsigned char R_out[WIDTH][HEIGHT], G_out[WIDTH][HEIGHT], B_out[WIDTH][HEIGHT];
    int center_x = WIDTH/2;
    int center_y = HEIGHT/2;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            double dx, dy;
            double radius, distortion, new_radius, theta;
            double x_src_d, y_src_d;
            dx = (double)(x - center_x) / center_x; // type cast to double because integer division
            dy = (double)(y - center_y) / center_y;
            radius = sqrt(dx*dx + dy*dy);
            
            distortion = (1 + k * radius * radius);
            theta = atan2(dy, dx);
            new_radius = (radius * distortion_factor) / (distortion * scaling_factor);
            
            x_src_d = (center_x + new_radius*cos(theta)*center_x); //floats cannot use as index
            y_src_d = (center_y + new_radius*sin(theta)*center_y); 
            int x_src = (int)(x_src_d); // type cast to int to be able to use as index
            int y_src = (int)(y_src_d);
            #ifdef DDEBUG
            printf("x: %d, y: %d -> x_src_f: %lf, y_src_f: %lf, x_src: %d, y_src: %d\n", x, y, x_src_d, y_src_d, x_src, y_src);
            printf("radius: %lf\n", radius);
            printf("dx: %lf dy: %lf\n", dx, dy);
            #endif
            if (x_src > 0 && x_src < WIDTH && y_src > 0 && y_src < HEIGHT)
            {
                R_out[x][y] = R[x_src][y_src];
                G_out[x][y] = G[x_src][y_src];
                B_out[x][y] = B[x_src][y_src];
            }
            else
            {
                R_out[x][y] = G_out[x][y] = B_out[x][y] = 0;
            }
            
        }
        
    }
    
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            R[x][y] = R_out[x][y];
            G[x][y] = G_out[x][y];
            B[x][y] = B_out[x][y];
        }
    }
            
    
}