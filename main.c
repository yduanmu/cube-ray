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
    double x, y, z;
} Point;

Point new_point(double x, double y, double z){
    Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    return p;
}

struct Point point_list[10];
//FLD, FRD, BRD, BLD, FLU, FRU, BRU, BLU, ORIG, DIR, ERROR
// 0    1    2    3    4    5    6    7    8     9    10

typedef struct Face{
    char side;
    int sidenum; //1, 2, or 3 (error)
    Point a, b, c;
} Face;

Face new_face(char side, int sidenum, Point a, Point b, Point c){
    Face f;
    f.side = side;
    f.sidenum = sidenum;
    f.a = a;
    f.b = b;
    f.c = c;
    return f;
}

typedef struct Distance{
    int index;
    double dist;
} Distance;

typedef struct PlaneEq{ //equation for plane
    double x_coeff, y_coeff, z_coeff;
    double equals;
} PlaneEq;

PlaneEq new_planeEq(double x_coeff, double y_coeff, double z_coeff, double equals){
    PlaneEq pe;
    pe.x_coeff = x_coeff;
    pe.y_coeff = y_coeff;
    pe.z_coeff = z_coeff;
    pe.equals = equals;
    return pe;
}

typedef struct LineEq{ //parametric equation for line
    double constant, t_coeff;
} LineEq;

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

bool contains(int target, int* vert_arr){
    for(int i = 0; i<3; i++){ //hard-coded to 3 since only 3 in vert_arr
        if(vert_arr[i] == target){
            return true;
        }
    }
    return false;
}

int cmp(const void* a, const void* b){
    const Distance* arga = a, *argb = b;
    return ((arga->dist > argb->dist) - (arga->dist < argb->dist));
}

Face nearest_face(void){ //nearest face to ray origin
    Face nearest = new_face('N', 3, point_list[0], point_list[0], point_list[0]); //init with just the FLD, 'N'ot a side
    Distance temp_list[8];
    int vert_arr[4] = {10, 10, 10, 10};

    //first find distance from all vertices
    for(int j=0; j<8; j++){
        temp_list[j].dist = sqrt(pow((point_list[8].x - point_list[j].x), 2) + 
                    pow((point_list[8].y - point_list[j].y), 2) + 
                    pow((point_list[8].z - point_list[j].z), 2));
        temp_list[j].index = j;
    }

    //sorting that list
    qsort(temp_list, 8, sizeof(Distance), cmp);

    //adding the first three into vert_arr (3 points make a plane, useful later)
    for(int i=0; i<3; i++){
        // printf("%d: %f %f %f\n", temp_list[i].index, point_list[temp_list[i].index].x, point_list[temp_list[i].index].y, point_list[temp_list[i].index].z);
        // printf("\t%f\n", temp_list[i].dist);
        vert_arr[i] = temp_list[i].index;
    }

    //checks whether the plane is a face of the cube, as well as which face it is
    //FLD, FRD, BRD, BLD, FLU, FRU, BRU, BLU, ORIG, DIR, ERROR
    // 0    1    2    3    4    5    6    7    8     9    10

    //Best-case: O(N)     Worst-case: O(N^7)
    if(contains(4, vert_arr)){ //FLU
        nearest.a = point_list[4];
        if(contains(0, vert_arr)){ //FLD
            nearest.b = point_list[0];
            if(contains(7, vert_arr)){ //BLU
                nearest.side = 'L';
                nearest.sidenum = 1;
                nearest.c = point_list[7];
            } else if(contains(5, vert_arr)){ //FRU
                nearest.side = 'F';
                nearest.sidenum = 1;
                nearest.c = point_list[5];
            } else {
                return nearest;
            }
        } else if(contains(7, vert_arr)){ //BLU
            nearest.b = point_list[7];
            if(contains(5, vert_arr)){ //FRU
                nearest.side = 'U';
                nearest.sidenum = 1;
                nearest.c = point_list[5];
            } else {
                return nearest;
            }
        } else {
            return nearest;
        }
    } else if(contains(6, vert_arr)) { //BRU
        nearest.a = point_list[6];
        if(contains(2, vert_arr)){ //BRD
            nearest.b = point_list[2];
            if(contains(7, vert_arr)){ //BLU
                nearest.side = 'B';
                nearest.sidenum = 1;
                nearest.c = point_list[7];
            } else if(contains(5, vert_arr)){ //FRU
                nearest.side = 'R';
                nearest.sidenum = 1;
                nearest.c = point_list[5];
            } else {
                return nearest;
            }
        } else if(contains(7, vert_arr)){ //BLU
            nearest.b = point_list[7];
            if(contains(5, vert_arr)){ //FRU
                nearest.side = 'U';
                nearest.sidenum = 2;
                nearest.c = point_list[5];
            } else {
                return nearest;
            }
        } else {
            return nearest;
        }
    } else if(contains(3, vert_arr)){ //BLD
        //FLD, FRD, BRD, BLD, FLU, FRU, BRU, BLU, ORIG, DIR, ERROR
        // 0    1    2    3    4    5    6    7    8     9    10
        nearest.a = point_list[3];
        if(contains(7, vert_arr)){ //BLU
            nearest.b = point_list[7];
            if(contains(0, vert_arr)){ //FLD
                nearest.side = 'L';
                nearest.sidenum = 2;
                nearest.c = point_list[0];
            } else if(contains(2, vert_arr)){ //BRD
                nearest.side = 'B';
                nearest.sidenum = 2;
                nearest.c = point_list[2];
            } else {
                return nearest;
            }
        } else if(contains(0, vert_arr)){ //FLD
            nearest.b = point_list[0];
            if(contains(2, vert_arr)){ //BRD
                nearest.side = 'D';
                nearest.sidenum = 1;
                nearest.c = point_list[2];
            } else {
                return nearest;
            }
        } else {
            return nearest;
        }
    } else if(contains(1, vert_arr)){ //FRD
        nearest.a = point_list[1];
        if(contains(5, vert_arr)){ //FRU
            nearest.b = point_list[5];
            if(contains(0, vert_arr)){ //FLD
                nearest.side = 'F';
                nearest.sidenum = 2;
                nearest.c = point_list[0];
            } else if(contains(2, vert_arr)){ //BRD
                nearest.side = 'R';
                nearest.sidenum = 2;
                nearest.c = point_list[2];
            } else {
                return nearest;
            }
        } else if(contains(0, vert_arr)){ //FLD
            nearest.b = point_list[0];
            if(contains(2, vert_arr)){ //BRD
                nearest.side = 'D';
                nearest.sidenum = 2;
                nearest.c = point_list[2];
            } else {
                return nearest;
            }
        } else {
            return nearest;
        }
    } else {
        return nearest;
    }

    return nearest;
}

PlaneEq find_planeEq(Face face){
    Point P = face.a;
    // printf("P: %f %f %f\n", P.x, P.y, P.z);
    Point Q = face.b;
    Point R = face.c;
    Point a, b; //these are not points, but rather the vectors between P->Q and P->R respectively
    Point n; //similar to above, this is the normal vector
    double equals;

    //first, find a & b
    a.x = Q.x - P.x;
    a.y = Q.y - P.y;
    a.z = Q.z - P.z;

    b.x = R.x - P.x;
    b.y = R.y - P.y;
    b.z = R.z - P.z;
    // printf("vector b: %f %f %f\n", b.x, b.y, b.z);

    //next, find the determinants of the cross product a x b in order to find the normal vector
    n.x = (a.y * b.z) - (a.y * b.z);
    n.y = -1 * ((a.x * b.z) - (b.x * a.z));
    n.z = (a.x * b.y) - (b.x * a.y);

    //last, find the "equals"
    equals = 0.0 - ((n.x * P.x) - (n.y * P.y) + (n.z * P.z));
    // printf("y coeff: %f\n", n.y);
    // printf("equals: %f\n", equals);

    return new_planeEq(n.x, n.y, n.z, equals);
}

//find intersection, if any, between the ray and the nearest face (given as a parametric equation)
Point find_intersection(Point orig, Point dir, PlaneEq p_eq){
    Point vec = new_point((orig.x - dir.x), (orig.y - dir.y), (orig.z - dir.z)); //not a point, but the direction vector for the line
    double t;
    LineEq x, y, z;
    Point intersection;

    //first, find the parametric equation of the line
    x.constant = orig.x;
    x.t_coeff = vec.x;
    // printf("x_const: %f\tx_t_coeff: %f\n", x.constant, x.t_coeff);

    y.constant = orig.y;
    y.t_coeff = vec.y;

    z.constant = orig.z;
    z.t_coeff = vec.z;

    //next, solve for t by replacing x, y, z in the plane equation with the parametric equations of each
    t = (p_eq.equals - ((p_eq.x_coeff * x.constant) + (p_eq.y_coeff * y.constant) + (p_eq.z_coeff * z.constant))) /
        ((p_eq.x_coeff * x.t_coeff) + (p_eq.y_coeff * y.t_coeff) + (p_eq.z_coeff * z.t_coeff));
    // printf("numerator: %f\n", (p_eq.equals - ((p_eq.x_coeff * x.constant) + (p_eq.y_coeff * y.constant) + (p_eq.z_coeff * z.constant))));
    // printf("t: %f\n", t);

    //then, find the point of intersection by plugging in t into the parametric equations
    intersection.x = x.constant + (x.t_coeff * t);
    intersection.y = y.constant + (y.t_coeff * t);
    intersection.z = z.constant + (z.t_coeff * t);

    //last, check if the point of intersection is actually at a cube face: does the point satisfy the plane's equation?


    return intersection;
}

int main(){
    char c_vert[14];
    char c_len[14];
    char r_orig[14];
    char r_dir[14];
    Point intersection;
    Face nearest;
    PlaneEq p_eq;
    do{
        printf("\n------------------------------------------------------------------\n");
        printf("x y z coordinates of the FLD vertex (eg, 0 0 0 for origin): "); //the corner at which front, left, and down intersect
        fgets(c_vert, 14, stdin);
        if(strstr(c_vert, "stop")){
            break;
        }
        make_point(c_vert, 0); //FLD
        //FLD, FRD, BRD, BLD, FLU, FRU, BRU, BLU, ORIG, DIR, ERROR
        // 0    1    2    3    4    5    6    7    8     9    10

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

        nearest = nearest_face();
        p_eq = find_planeEq(nearest);
        if(nearest.side != 'N'){
            printf("Nearest face: %c%d\t", nearest.side, nearest.sidenum);
            // printf("p_equals: %f\n", p_eq.equals);
            intersection = find_intersection(point_list[8], point_list[9], p_eq);
            printf("Intersection: %f %f %f\n", intersection.x, intersection.y, intersection.z);
        } else {
            printf("No valid nearest face.\n");
        }

        printf("------------------------------------------------------------------\n");
    } while(true);
}

/*
gcc -o Main -std=c99 -Wall -Werror -fsanitize=address main.c
*/