#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem> // C++17
#include "WeatherStatistics.h"
using namespace std::filesystem;

/// <summary>
/// Searches the in-memory map for the associated barometric pressure
/// </summary>
/// <param name="datetime"></param>
/// <returns></returns>
float WeatherData::GetBarometricPressure(tm* datetime) 
{
    auto pressure = m_weatherMap.lower_bound(mktime(datetime));
    if (pressure != m_weatherMap.end())
    {
        return pressure->second.barometricPressure;
    };
    return -1.0;
}

/// <summary>
/// Sets the base path for the data files
/// </summary>
/// <param name="p"></param>
void WeatherData::SetResourcePath(string p) 
{
    m_resourcePath = p;
}

/// <summary>
/// Loads weather data into a memory using a map
/// </summary>
/// <param name="yearBegin"></param>
/// <param name="yearEnd"></param>
void WeatherData::LoadWeatherData(int yearBegin, int yearEnd) 
{
    if (m_resourcePath.empty())
    {
        cerr << "Set resource path first." << endl;
        return;
    }

    for (int year = yearBegin; year <= yearEnd; year++)
    {
        ostringstream fileNameStream;
        fileNameStream << m_resourcePath << "Environmental_Data_Deep_Moor_" << year << ".txt";
        string filename = fileNameStream.str();
        cout << "Loading " << filename << endl;

        fstream dataStream;
        try
        {
            dataStream.open(filename);
            if (dataStream.fail())
            {
                throw exception("datastream didn't open");
            }
        }
        catch(exception& e)
        {
            cout << e.what() << endl;
            continue;
        }
        
        string line;
        getline(dataStream, line); // discard top line with headers
        while (getline(dataStream, line))
        {
            string date, time;
            WeatherDataUnit wdu;

            istringstream buffer(line);

            buffer >> date >> time >> wdu.airTemp >> wdu.barometricPressure >> wdu.dewPoint >> wdu.relativeHumidity >> wdu.windDirection >> wdu.windGust >> wdu.windSpeed;
            
            time_t epochTime;
            if (epochTime = ConvertDateTimeToEpoch(date, time) < 0)
            {
                cerr << "WeatherData not loaded." << endl;
                continue; //try the next one
            }

            m_weatherMap[epochTime] =  wdu;
        }
        dataStream.close();
    }
}

time_t WeatherData::ConvertDateTimeToEpoch(string date, string time)
{
    int year, month, day = 0;
    // sscanf takes a c_style string and parses it into component pieces.  It returns a value of the number of pieces parsed successfully.
    // sscanf was unsafe.  used sscanf_s
    // date = 2012_01_01
    if (sscanf_s(date.c_str(), "%d_%d_%d", &year, &month, &day) != 3)
    {
        cerr << "ERROR: Failed to parse date string " << date << endl;
        return -1;
    }

    int hours, minutes, seconds = 0;
    if (sscanf_s(time.c_str(), "%d:%d:%d", &hours, &minutes, &seconds) != 3)
    {
        cerr << "ERROR: Failed to parse time string " << date << endl;
        return -2;
    }

    // create a tm structure that mktime willl understand
    struct tm dateTime = {};
    dateTime.tm_year = year - 1900; // years since 1900
    dateTime.tm_mon = month - 1;      // months since January
    dateTime.tm_mday = day;          // day of the month
    dateTime.tm_hour = hours;          // hours since midnight
    dateTime.tm_min = minutes;           // minutes after the hour
    dateTime.tm_sec = seconds;           // seconds after the minute

    return mktime(&dateTime);
}
float WeatherData::GetBarometricCoefficient(tm* starttime, tm* endtime) 
{
    if (m_weatherMap.size() == 0)
    {
        return -9999;
    }
    Point* pp1 = new Point();
    Point* pp2 = new Point();
    pp1->x = mktime(starttime);
    pp1->y = GetBarometricPressure(starttime);
    pp2->x = mktime(endtime);
    pp2->y = GetBarometricPressure(endtime);

    float result = m_slopeCalculator->calculateSlope(pp1, pp2);
    pp1 = nullptr;
    pp2 = nullptr;
    delete pp1;
    delete pp2;
    return result;
}