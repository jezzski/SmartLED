/** \file
 * Description: Contains function definitions to approximate dawn and dusk times based on latitude and longitude.
 * This article was first published on Quantitative Ecology, and adapted from R-bloggers.
 * \author: Shipra Vaidya
 * \date last modified: 4/26/2020
 */

#include "dawndusk.h"

//This article was first published on Quantitative Ecology, and adapted from R-bloggers.

double degToRad(double deg)
{
	return (pi * deg / 180.0);
}

int sign(double x)
{
	if(x > 0)
		return 1;
	if(x < 0)
		return -1;
	return 0; //no zero value for longitude
}


double dawnCalc(int day, int month, int year, double latitude, double longitude)
{
	int val1 = floor(275 * month / 9);
	int val2 = floor((month + 9) / 12);
	int val3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
	int dayVal = val1 - (val2 * val3) + day - 30;
	
	double epsilon = degToRad(planeDist);
	
	//latitude in radians, longitude in degrees
	double L = degToRad(latitude);
	
	int signLong = sign(longitude);
    
    double timezone = -4 * (abs(longitude) % 15) * signLong;
	double theta = 2 * pi / 365.25 * (dayVal - 80);
	
	double sinVal = distSun * sin(theta) * sin(epsilon); //off
	double sqrtVal = sqrt(pow(distSun, 2) - pow(sinVal, 2));

	double t = 1440/(2*pi) * acos((radius - sinVal * sin(L)) / (sqrtVal * cos(L)));

	double radAdj = t + 5.0;
	double noonAdj = 720 - 10 * sin(4 * pi * (dayVal - 80) / 365.25) + 8 * sin(2 * pi* dayVal / 365.25);
	
	double dawn = (noonAdj - radAdj + timezone) / 60;
	//double dusk = (noonAdj + radAdj + timezone) / 60;	
	
	return dawn;
}

double duskCalc(int day, int month, int year, double latitude, double longitude)
{
	int val1 = floor(275 * month / 9);
	int val2 = floor((month + 9) / 12);
	int val3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
	int dayVal = val1 - (val2 * val3) + day - 30;

	double epsilon = degToRad(planeDist);

	//latitude in radians, longitude in degrees
	double L = degToRad(latitude);

	int signLong = sign(longitude);

	double timezone = -4 * (abs(longitude) % 15) * signLong;
	double theta = 2 * pi / 365.25 * (dayVal - 80);

	double sinVal = distSun * sin(theta) * sin(epsilon); //off
	double sqrtVal = sqrt(pow(distSun, 2) - pow(sinVal, 2));

	double t = 1440 / (2 * pi) * acos((radius - sinVal * sin(L)) / (sqrtVal * cos(L)));

	double radAdj = t + 5.0;
	double noonAdj = 720 - 10 * sin(4 * pi * (dayVal - 80) / 365.25) + 8 * sin(2 * pi* dayVal / 365.25);
	
	//double dawn = (noonAdj - radAdj + timezone) / 60;
	double dusk = (noonAdj + radAdj + timezone) / 60;	
	
	return dusk;
}