/**
 * Melissa Duanmu mduanmu@u.rochester.edu
 * Last modified: 6 Feb 2025
 * cube-ray main.c
gcc -o Main -std=c99 -Wall -Werror -fsanitize=address main.c
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

typedef struct Point{
    float x, y, z;
} Point;

Point new_point(float x, float y, float z){
    Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    return p;
}

struct Point point_list[10];
//FLD, FRD, BRD, BLD, FLU, FRU, BRU, BLU, ORIG, DIR
// 0    1    2    3    4    5    6    7    8     9

void make_point(char* str, int i){
    double x, y, z;
    
    char * ptr = strtok(str, " "); //separate string on " " and put into x, y, z
    x = atof(ptr);
    ptr = strtok(NULL, " ");
    y = atof(ptr);
    ptr = strtok(NULL, " ");
    z = atof(ptr);

    point_list[i] = new_point(x, y, z);
}

int nearest_vertex(void){ //nearest vertex to ray origin
    int i = 0; //init with FLD
    //for each x y z, abs(abs(origin) - abs(vertex)) to get magnitude
    double dist = sqrt(pow((point_list[8].x - point_list[0].x), 2) + 
                       pow((point_list[8].y - point_list[0].y), 2) + 
                       pow((point_list[8].z - point_list[0].z), 2)); //init with FLD
    double temp;
    for(int j=1; j<=7; j++){
        temp = sqrt(pow((point_list[8].x - point_list[j].x), 2) + 
                    pow((point_list[8].y - point_list[j].y), 2) + 
                    pow((point_list[8].z - point_list[j].z), 2));
        if(temp < dist){
            dist = temp;
            i = j;
        }
    }

    return i; //index of nearest point
}

int main(){
    char c_vert[14];
    char c_len[14];
    char r_orig[14];
    char r_dir[14];
    do{
        printf("x y z coordinates of the FLD vertex (eg, 0 0 0 for origin): "); //the corner at which front, left, and down intersect
        fgets(c_vert, 14, stdin);
        if(strstr(c_vert, "stop")){
            break;
        }
        make_point(c_vert, 0); //FLD
        //FLD, FRD, BRD, BLD, FLU, FRU, BRU, BLU, ORIG, DIR
        // 0    1    2    3    4    5    6    7    8     9

        printf("Length of cube: ");
        fgets(c_len, 14, stdin);
        point_list[1] = new_point(point_list[0].x + atof(c_len), point_list[0].y, point_list[0].z); //FRD
        point_list[2] = new_point(point_list[0].x + atof(c_len), point_list[0].y, point_list[0].z - atof(c_len)); //BRD
        point_list[3] = new_point(point_list[0].x, point_list[0].y, point_list[0].z - atof(c_len)); //BLD
        point_list[4] = new_point(point_list[0].x, point_list[0].y + atof(c_len), point_list[0].z); //FLU
        point_list[5] = new_point(point_list[0].x + atof(c_len), point_list[0].y + atof(c_len), point_list[0].z); //FRU
        point_list[6] = new_point(point_list[0].x + atof(c_len), point_list[0].y + atof(c_len), point_list[0].z - atof(c_len)); //BRU
        point_list[7] = new_point(point_list[0].x, point_list[0].y + atof(c_len), point_list[0].z - atof(c_len)); //BLU

        // printf("Cube coords:\n");
        // for(int i = 0; i<=7; i++){
        //     printf("%f %f %f\n", point_list[i].x, point_list[i].y, point_list[i].z);
        // }

        printf("x y z coordinates of ray origin: ");
        fgets(r_orig, 14, stdin);
        make_point(r_orig, 8); //ORIG

        printf("x y z coordinates of ray direction: "); //no need for spherical—— two points make a line.
        fgets(r_dir, 14, stdin);
        make_point(r_dir, 9); //DIR

        printf("Nearest vertex: %d\n", nearest_vertex());
    } while(true);
}

/*
gcc -o Main -std=c99 -Wall -Werror -fsanitize=address main.c
*/