#include <iostream>
#include "WeatherStatistics.h"

int main()
{
    string resourcepath = "../../../../../../codeclinic/Ex_Files_Code_Clinic_C_Plus_Plus/Exercise Files/CH01/resources/";
    LinearRegression* lr = new LinearRegression();
    WeatherData* wd = new WeatherData(lr);
    wd->SetResourcePath(resourcepath);
    wd->LoadWeatherData(2012, 2015);
    tm starttime;
    tm endtime;
    starttime.tm_year = 2013;
    starttime.tm_mon = 1;
    starttime.tm_hour = 0;
    starttime.tm_mday = 1;
    starttime.tm_min = 0;
    starttime.tm_sec = 0;

    endtime = starttime;
    endtime.tm_hour = 3;

    // & passing in a pointer to the struct tm
    float coefficient = wd->GetBarometricCoefficient(&starttime, &endtime);
    if (coefficient > 0)
    {
        cout << "It's a sunny day." << endl;
    }
    else if ((int)coefficient == -9999)
    {
        cout << "Something went wrong." << endl;
    }
    else
    {
        cout << "It's a rainy day." << endl;
    }

    lr = nullptr;
    wd = nullptr;
    delete lr;
    delete wd;
}


