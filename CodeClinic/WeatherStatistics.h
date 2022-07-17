#pragma once
/*
*   Basic algorithm steps:
    Read in data from files
    Read in dates from user
    Find corresponding dates and calculate the slope the defines the barometric pressure
*/

/* data format tab separated values
date        time    	Air_Temp	Barometric_Press	Dew_Point	Relative_Humidity	Wind_Dir	Wind_Gust	Wind_Speed
2012_01_01  00:02:14	34.30	    30.50	            26.90	    74.20	            346.40	    11.00	    3.60
*/
#include <ctime>
#include <map>
#include <stdexcept>

using namespace std;

struct WeatherDataUnit
{
    time_t datetime;
    float airTemp;
    float barometricPressure;
    float dewPoint;
    float relativeHumidity;
    float windDirection;
    float windGust;
    float windSpeed;
    virtual ~WeatherDataUnit() {};
};

struct Point
{
    float x = 0;
    float y = 0;
    float z = 0;
    virtual ~Point() {};
};

class ISlopeCalculator
{
    public:
        virtual float calculateSlope(Point* x1, Point* x2) = 0;
        virtual float calculateSlope(Point* points[], int size) = 0;
        virtual ~ISlopeCalculator() {};
};

class WeatherData
{       
    WeatherData() = delete;
    public:
        WeatherData(ISlopeCalculator* sc)
            :m_slopeCalculator(sc)
        {  
           
        };
        float GetBarometricPressure(tm* datetime);
        void SetResourcePath(string p);
        void LoadWeatherData(int yearBegin, int yearEnd);
        time_t ConvertDateTimeToEpoch(string date, string time);
        float GetBarometricCoefficient(tm* starttime, tm* endtime);
        virtual ~WeatherData() {};
    private:
        std::pmr::map<time_t, WeatherDataUnit> m_weatherMap;
        ISlopeCalculator* m_slopeCalculator;
        string m_resourcePath;
};

// Inheritance of classes is private by default, in order to use the interface I must make the inheritance os ISlopeCalculator
class BasicSlope : public ISlopeCalculator
{
    public:
        float calculateSlope(Point* p1, Point* p2) override
        {
            float rise = p2->y - p1->y;
            float run = p2->x - p1->x;
            return rise / run;
        }
    };

class LinearRegression : public ISlopeCalculator
{
    public:
        float calculateSlope(Point* p1, Point* p2) override
        {
            float rise = p2->y - p1->y;
            float run = p2->x - p1->x;
            return rise / run;
        }

        float calculateSlope(Point* points[], int size) override
        {
            // for now return basic slope
            return calculateSlope(points[0], points[size - 1]);
        }
};