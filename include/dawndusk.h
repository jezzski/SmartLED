#ifndef DAWNDUSK_H
#define DAWNDUSK_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 * @brief Size of radius of Earth, used in dawn/dusk calculation.
 * 
 */
#define radius 6378

/**
 * @brief Value of pi, used in dawn/dusk calculations.
 * 
 */
#define pi 3.1415926

/**
 * @brief Distance between x-y plane and ecliptic plane, used in dawn/dusk calculations.
 * 
 */
#define planeDist 23.45

/**
 * @brief Distance from Earth to sun, used in dawn/dusk calculations.
 * 
 */
#define distSun 149598000

/**
 * @brief Convert degrees to radians.
 * 
 * @param deg Latitude or longitude value in degrees.
 * @return double Returns converted value in radians.
 */
double degToRad(double deg);

/**
 * @brief Checks sign of given value. If longitude value is negative, an offset is taken into account for dawn/duck calculations.
 * 
 * @param x Value of longitude.
 * @return int Returns 1 if positive, -1 if negative, to be multiplied in offset calculation.
 */
int sign(double x);

/**
 * @brief Calculation of approximate dawn time based on given day and location.
 * 
 * @param day Numerical value, day of month.
 * @param month Numerical value, month in year.
 * @param year Numerical value, year.
 * @param latitude Degree value of latitude from user's location.
 * @param longitude Degree value of longitude from user's location.
 * @return double Returns approximate time of dawn on given date at given location. 
 */
double dawnCalc(int day, int month, int year, double latitude, double longitude);

/**
 * @brief Calculation of approximate dusk time based on given day and location
 * 
 * @param day Numerical value, day of month.
 * @param month Numerical value, month in year.
 * @param year Numerical value, year.
 * @param latitude Degree value of latitude from user's location.
 * @param longitude Degree value of longitude from user's location.
 * @return double Returns approximate time of dusk on given date at given location.
 */
double duskCalc(int day, int month, int year, double latitude, double longitude);

#endif