/**
 * Melissa Duanmu mduanmu@u.rochester.edu
 * Last modified: 10 Feb 2025
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
    bool intersection; //only used in the point that could potentially be an intersection
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
    for(int i = 0; i<4; i++){ //hard-coded to 4 since only 4 in vert_arr
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

int cmpnum(const void* a, const void* b){
    double arg1 = *(const int*)a;
    double arg2 = *(const int*)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

double point_plane_dist(Point p, PlaneEq f){
    double a = f.x_coeff;
    double b = f.y_coeff;
    double c = f.z_coeff;
    double d = -1.0 * f.equals;
    double denom = (sqrt(pow(a, 2) + pow(b, 2) + pow(c, 2)));

    return (fabs((a * p.x) + (b * p.y) + (c * p.z) + d))/denom;
}

PlaneEq find_planeEq(Face face){
    Point P = face.a;
    Point Q = face.b;
    Point R = face.c;
    Point a, b; //these are not points, but rather the vectors between P->Q and P->R respectively
    Point n; //similar to above, this is the normal vector
    double equals;

    // printf("P: %f %f %f\n", P.x, P.y, P.z);
    // printf("Q: %f %f %f\n", Q.x, Q.y, Q.z);
    // printf("R: %f %f %f\n", R.x, R.y, R.z);

    //first, find a & b
    a.x = Q.x - P.x;
    a.y = Q.y - P.y;
    a.z = Q.z - P.z;

    b.x = R.x - P.x;
    b.y = R.y - P.y;
    b.z = R.z - P.z;

    // printf("a: <%f %f %f>\n", a.x, a.y, a.z);
    // printf("b: <%f %f %f>\n", b.x, b.y, b.z);

    //next, find the determinants of the cross product a x b in order to find the normal vector
    n.x = (a.y * b.z) - (b.y * a.z);
    n.y = -1 * ((a.x * b.z) - (b.x * a.z));
    n.z = (a.x * b.y) - (b.x * a.y);

    // printf("normal vector: <%f %f %f>\n", n.x, n.y, n.z);

    //last, find the "equals"
    equals = 0.0 - ((n.x * P.x) + (n.y * P.y) + (n.z * P.z));

    // printf("Plane equation: %fx - %fy + %fz = %f\n", n.x, n.y, n.z, equals);

    return new_planeEq(n.x, n.y, n.z, equals);
}

bool is_vertex(Point a){ //is this point a vertex
    for(int i = 0; i<8; i++){
        if(fabs(fabs(a.x) - fabs(point_list[i].x)) < 1e-6 &&
           fabs(fabs(a.y) - fabs(point_list[i].y)) < 1e-6 &&
           fabs(fabs(a.z) - fabs(point_list[i].z)) < 1e-6){
            return true;
        }
    }
    return false;
}

//FLD, FRD, BRD, BLD, FLU, FRU, BRU, BLU, ORIG, DIR, ERROR
// 0    1    2    3    4    5    6    7    8     9    10
Face nearest_face(void){ //nearest face to ray origin
    Distance temp_list[8];
    int vert_arr[] = {10, 10, 10, 10};
    Face nearest = new_face('N', 3, point_list[8], point_list[8], point_list[8]);

    //first find distance from all vertices
    for(int j=0; j<8; j++){
        temp_list[j].dist = sqrt(pow((point_list[8].x - point_list[j].x), 2) + 
                    pow((point_list[8].y - point_list[j].y), 2) + 
                    pow((point_list[8].z - point_list[j].z), 2));
        temp_list[j].index = j;
    }

    //sorting that list
    qsort(temp_list, 8, sizeof(Distance), cmp);

    //if first and last are roughly equal, then orig is equidistant
    //so use dir instead of orig
    if(fabs(temp_list[0].dist - temp_list[7].dist) < 1e-6 || is_vertex(point_list[8])){
        // printf("orig equidistant or at vertex\n");

        for(int j=0; j<8; j++){
            temp_list[j].dist = sqrt(pow((point_list[9].x - point_list[j].x), 2) + 
                        pow((point_list[9].y - point_list[j].y), 2) + 
                        pow((point_list[9].z - point_list[j].z), 2));
            temp_list[j].index = j;
        }
        qsort(temp_list, 8, sizeof(Distance), cmp);
    }

    //adding the first four into vert_arr
    // printf("nearest vertex indices: ");
    for(int i=0; i<4; i++){
        // printf("%d: %f %f %f\n", temp_list[i].index, point_list[temp_list[i].index].x, point_list[temp_list[i].index].y, point_list[temp_list[i].index].z);
        // printf("\t%f\n", temp_list[i].dist);
        vert_arr[i] = temp_list[i].index;
        // printf("%d ", vert_arr[i]);
    }
    // printf("\n");

    //checks whether the plane is a face of the cube, as well as which face it is
    //FLD, FRD, BRD, BLD, FLU, FRU, BRU, BLU, ORIG, DIR, ERROR
    // 0    1    2    3    4    5    6    7    8     9    10
    if(contains(0, vert_arr)){ //FLD
        nearest.a = point_list[0];
        if(contains(4, vert_arr)){ //FLU
            nearest.b = point_list[4];
            if(contains(7, vert_arr)){ //BLU
                nearest.c = point_list[7];
                nearest.side = 'L';
            } else if (contains(5, vert_arr)){ //FRU
                nearest.c = point_list[5];
                nearest.side = 'F';
            } else {
                return nearest;
            }
        } else if(contains(2, vert_arr)){ //BRD
            nearest.b = point_list[2];
            if(contains(3, vert_arr)){ //BLD
                nearest.c = point_list[3];
                nearest.side = 'D';
            } else {
                return nearest;
            }
        } else {
            return nearest;
        }
    } else if(contains(6, vert_arr)){ //BRU
        nearest.a = point_list[6];
        if(contains(4, vert_arr)){ //FLU
            nearest.b = point_list[4];
            if(contains(7, vert_arr)){ //BLU
                nearest.c = point_list[7];
                nearest.side = 'U';
            } else {
                return nearest;
            }
        } else if(contains(2, vert_arr)){ //BRD
            nearest.b = point_list[2];
            if(contains(7, vert_arr)){ //BLD
                nearest.c = point_list[7];
                nearest.side = 'B';
            } else if (contains(1, vert_arr)){ //FRD
                nearest.c = point_list[1];
                nearest.side = 'R';
            } else {
                return nearest;
            }
        } else {
            return nearest;
        }
    } else {
        return nearest;
    }

    //Best-case: O(N)     Worst-case: O(N^7)
    // if(contains(4, vert_arr)){ //FLU
    //     nearest.a = point_list[4];
    //     if(contains(0, vert_arr)){ //FLD
    //         nearest.b = point_list[0];
    //         if(contains(7, vert_arr)){ //BLU
    //             nearest.side = 'L';
    //             nearest.sidenum = 1;
    //             nearest.c = point_list[7];
    //         } else if(contains(5, vert_arr)){ //FRU
    //             nearest.side = 'F';
    //             nearest.sidenum = 1;
    //             nearest.c = point_list[5];
    //         } else {
    //             return nearest;
    //         }
    //     } else if(contains(7, vert_arr)){ //BLU
    //         nearest.b = point_list[7];
    //         if(contains(5, vert_arr)){ //FRU
    //             nearest.side = 'U';
    //             nearest.sidenum = 1;
    //             nearest.c = point_list[5];
    //         } else {
    //             return nearest;
    //         }
    //     } else {
    //         return nearest;
    //     }
    // } else if(contains(6, vert_arr)) { //BRU
    //     nearest.a = point_list[6];
    //     if(contains(2, vert_arr)){ //BRD
    //         nearest.b = point_list[2];
    //         if(contains(7, vert_arr)){ //BLU
    //             nearest.side = 'B';
    //             nearest.sidenum = 1;
    //             nearest.c = point_list[7];
    //         } else if(contains(5, vert_arr)){ //FRU
    //             nearest.side = 'R';
    //             nearest.sidenum = 1;
    //             nearest.c = point_list[5];
    //         } else {
    //             return nearest;
    //         }
    //     } else if(contains(7, vert_arr)){ //BLU
    //         nearest.b = point_list[7];
    //         if(contains(5, vert_arr)){ //FRU
    //             nearest.side = 'U';
    //             nearest.sidenum = 2;
    //             nearest.c = point_list[5];
    //         } else {
    //             return nearest;
    //         }
    //     } else {
    //         return nearest;
    //     }
    // } else if(contains(3, vert_arr)){ //BLD
    //     //FLD, FRD, BRD, BLD, FLU, FRU, BRU, BLU, ORIG, DIR, ERROR
    //     // 0    1    2    3    4    5    6    7    8     9    10
    //     nearest.a = point_list[3];
    //     if(contains(7, vert_arr)){ //BLU
    //         nearest.b = point_list[7];
    //         if(contains(0, vert_arr)){ //FLD
    //             nearest.side = 'L';
    //             nearest.sidenum = 2;
    //             nearest.c = point_list[0];
    //         } else if(contains(2, vert_arr)){ //BRD
    //             nearest.side = 'B';
    //             nearest.sidenum = 2;
    //             nearest.c = point_list[2];
    //         } else {
    //             return nearest;
    //         }
    //     } else if(contains(0, vert_arr)){ //FLD
    //         nearest.b = point_list[0];
    //         if(contains(2, vert_arr)){ //BRD
    //             nearest.side = 'D';
    //             nearest.sidenum = 1;
    //             nearest.c = point_list[2];
    //         } else {
    //             return nearest;
    //         }
    //     } else {
    //         return nearest;
    //     }
    // } else if(contains(1, vert_arr)){ //FRD
    //     nearest.a = point_list[1];
    //     if(contains(5, vert_arr)){ //FRU
    //         nearest.b = point_list[5];
    //         if(contains(0, vert_arr)){ //FLD
    //             nearest.side = 'F';
    //             nearest.sidenum = 2;
    //             nearest.c = point_list[0];
    //         } else if(contains(2, vert_arr)){ //BRD
    //             nearest.side = 'R';
    //             nearest.sidenum = 2;
    //             nearest.c = point_list[2];
    //         } else {
    //             return nearest;
    //         }
    //     } else if(contains(0, vert_arr)){ //FLD
    //         nearest.b = point_list[0];
    //         if(contains(2, vert_arr)){ //BRD
    //             nearest.side = 'D';
    //             nearest.sidenum = 2;
    //             nearest.c = point_list[2];
    //         } else {
    //             return nearest;
    //         }
    //     } else {
    //         return nearest;
    //     }
    // } else {
    //     return nearest;
    // }

    // printf("Face: %c\n", nearest.side);
    return nearest;
}

//find intersection, if any, between the ray and the nearest face (given as a parametric equation)
Point find_intersection(Point orig, Point dir, PlaneEq p_eq, Face nearest){
    Point vec = new_point((dir.x - orig.x), (dir.y - orig.y), (dir.z - orig.z)); //not a point, but the direction vector for the line
    double t;
    LineEq x, y, z;
    Point intersection;
    double denominator;
    double plane_x[3];
    double plane_y[3];
    double plane_z[3];
    double d = -1 * p_eq.equals;
    Point a, b; //these are not points, but rather the vectors between orig->intersection and orig->dir respectively

    //first, find the parametric equation of the line
    x.constant = orig.x;
    x.t_coeff = vec.x;
    // printf("x_const: %f\tx_t_coeff: %f\n", x.constant, x.t_coeff);

    y.constant = orig.y;
    y.t_coeff = vec.y;
    // printf("y_const: %f\ty_t_coeff: %f\n", y.constant, y.t_coeff);

    z.constant = orig.z;
    z.t_coeff = vec.z;
    // printf("z_const: %f\tz_t_coeff: %f\n", z.constant, z.t_coeff);

    //next, solve for t by replacing x, y, z in the plane equation with the parametric equations of each
    denominator = (p_eq.x_coeff * x.t_coeff) + (p_eq.y_coeff * y.t_coeff) + (p_eq.z_coeff * z.t_coeff);
    
    if(fabs(denominator) >= 1e-6){
        t = (d - ((p_eq.x_coeff * x.constant) + (p_eq.y_coeff * y.constant) + (p_eq.z_coeff * z.constant))) /
            denominator;
        // printf("t: %f\n", t);
    } else {
        intersection.intersection = false;
        return intersection;
    }

    //then, find the point of intersection by plugging in t into the parametric equations
    intersection.x = x.constant + (x.t_coeff * t);
    intersection.y = y.constant + (y.t_coeff * t);
    intersection.z = z.constant + (z.t_coeff * t);

    //after, check if the point of intersection is actually at a cube face: does the point satisfy the plane's equation?
    plane_x[0] = nearest.a.x;
    plane_x[1] = nearest.b.x;
    plane_x[2] = nearest.c.x;

    plane_y[0] = nearest.a.y;
    plane_y[1] = nearest.b.y;
    plane_y[2] = nearest.c.y;

    plane_z[0] = nearest.a.z;
    plane_z[1] = nearest.b.z;
    plane_z[2] = nearest.c.z;

    qsort(plane_x, sizeof(plane_x)/sizeof(plane_x[0]), sizeof(double), cmpnum);
    qsort(plane_y, sizeof(plane_y)/sizeof(plane_y[0]), sizeof(double), cmpnum);
    qsort(plane_z, sizeof(plane_z)/sizeof(plane_z[0]), sizeof(double), cmpnum);

    // printf("largest x: %f\tsmallest x: %f\n", plane_x[0], plane_x[2]);
    // printf("largest y: %f\tsmallest y: %f\n", plane_y[0], plane_y[2]);
    // printf("largest z: %f\tsmallest z: %f\n", plane_z[0], plane_z[2]);
    // printf("possible intersection: %f %f %f\n", intersection.x, intersection.y, intersection.z);

    if(intersection.x <= plane_x[2] && intersection.x >= plane_x[0] &&
       intersection.y <= plane_y[2] && intersection.y >= plane_y[0] &&
       intersection.z <= plane_z[2] && intersection.z >= plane_z[0]){
        intersection.intersection = true;
    } else {
        intersection.intersection = false;
    }

    //lastly, what we found was the intersection of a *line*—— what we want is a ray instead.
    //(orig->intersection) & b (orig->dir)
    a.x = intersection.x - orig.x;
    a.y = intersection.y - orig.y;
    a.z = intersection.z - orig.z;
    b.x = dir.x - orig.x;
    b.y = dir.y - orig.y;
    b.z = dir.z - orig.z;
    if(((a.x * b.x) + (a.y * b.y) + (a.z * b.z)) >= 1e-6) { //dot product of orig->intersection and orig->dir
        intersection.intersection = true;
    } else {
        intersection.intersection = false;
    }

    return intersection;
}

int main(){
    char c_vert[14];
    char c_len[14];
    // double len;
    // char c_vert2[14];
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

        // printf("Coordinates of the BRD vertex: ");
        // fgets(c_vert2, 14, stdin);
        // make_point(c_vert, 1); //FRD

        // len = point_list[0].x - point_list[2].x;
        // point_list[2] = new_point(point_list[0].x + len, point_list[0].y, point_list[0].z - len); //BRD
        // point_list[3] = new_point(point_list[0].x, point_list[0].y, point_list[0].z - len); //BLD
        // point_list[4] = new_point(point_list[0].x, point_list[0].y + len, point_list[0].z); //FLU
        // point_list[5] = new_point(point_list[0].x + len, point_list[0].y + len, point_list[0].z); //FRU
        // point_list[6] = new_point(point_list[0].x + len, point_list[0].y + len, point_list[0].z - len); //BRU
        // point_list[7] = new_point(point_list[0].x, point_list[0].y + len, point_list[0].z - len); //BLU

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
            // printf("Nearest face: %c%d\t", nearest.side, nearest.sidenum);
            // printf("p_equals: %f\n", p_eq.equals);
            intersection = find_intersection(point_list[8], point_list[9], p_eq, nearest);
            if(intersection.intersection){
                printf("Closest intersection is at %c face: %f %f %f\n", nearest.side, intersection.x, intersection.y, intersection.z);
            } else {
                printf("No valid intersection.\n");
            }
        } else {
            printf("No valid intersection.\n");
            // printf("No valid nearest face.\n");
        }

        printf("------------------------------------------------------------------\n");
    } while(true);
}

/*
gcc -o Main -std=c99 -Wall -Werror -fsanitize=address main.c
*/