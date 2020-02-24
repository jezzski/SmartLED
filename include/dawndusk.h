#ifndef DAWNDUSK_H
#define DAWNDUSK_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#define radius 6378
#define pi 3.1415926
#define planeDist 23.45
#define distSun 149598000

//function definitions
double degToRad(double deg);
int sign(double x);
double dawnCalc(int day, int month, int year, double latitude, double longitude);
double duskCalc(int day, int month, int year, double latitude, double longitude);

#endif