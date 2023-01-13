#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <math.h>
#include <sstream>
#include <iomanip>

// GIS record structure
struct GISRecord
{
    int id;
    int latitude_degrees;
    int latitude_minutes;
    double latitude_seconds;
    char latitude_hemisphere;
    int longitude_degrees;
    int longitude_minutes;
    double longitude_seconds;
    char longitude_hemisphere;
    std::vector<std::string> optional_fields;
};

// Generate a random GIS record
GISRecord generateRecord()
{
    // Generate a random ID
    int id = rand();

    // Generate random latitude and longitude coordinates
    double latitude = 90.0 * (double)rand() / RAND_MAX;
    if (rand() % 2 == 0) latitude *= -1;
    char latitude_hemisphere = (latitude < 0) ? 'S' : 'N';

    double longitude = 180.0 * (double)rand() / RAND_MAX;
    if (rand() % 2 == 0) longitude *= -1;
    char longitude_hemisphere = (longitude < 0) ? 'W' : 'E';

    // Convert the latitude and longitude coordinates to DMS format
    int latitude_degrees = std::floor(latitude);
    double latitude_minutes_seconds = std::fmod(latitude, 1) * 60;
    int latitude_minutes = std::floor(latitude_minutes_seconds);
    double latitude_seconds = std::floor(std::fmod(latitude_minutes_seconds, 1) * 60);

    int longitude_degrees = std::floor(longitude);
    double longitude_minutes_seconds = std::fmod(longitude, 1) * 60;
    int longitude_minutes = std::floor(longitude_minutes_seconds);
    double longitude_seconds = std::floor(std::fmod(latitude_minutes_seconds, 1) * 60);

    // Generate 17 random optional fields
    std::vector<std::string> optional_fields;
    for (int i = 0; i < 17; i++)
    {
        optional_fields.push_back("field_" + std::to_string(rand()));
    }

    // Return a GIS record with the generated data
    return GISRecord{id, std::abs(latitude_degrees), std::abs(latitude_minutes), std::abs(latitude_seconds), latitude_hemisphere,
                     std::abs(longitude_degrees), std::abs(longitude_minutes), std::abs(longitude_seconds), longitude_hemisphere, optional_fields};
}

std::string s(int i, int size = 2){
     std::stringstream ss;
    ss << std::setw(size) << std::setfill('0') << i;
    return ss.str();
}

int main()
{
    // Seed the random number generator
    std::srand(std::time(nullptr));

    // Open a file for output
    std::ofstream file("CO_big.txt");

    file << "FEATURE_ID|FEATURE_NAME|FEATURE_CLASS|STATE_ALPHA|STATE_NUMERIC|COUNTY_NAME|COUNTY_NUMERIC|PRIMARY_LAT_DMS|PRIM_LONG_DMS|PRIM_LAT_DEC|PRIM_LONG_DEC|SOURCE_LAT_DMS|SOURCE_LONG_DMS|SOURCE_LAT_DEC|SOURCE_LONG_DEC|ELEV_IN_M|ELEV_IN_FT|MAP_NAME|DATE_CREATED|DATE_EDITED" << std::endl;

    // Generate and output 10 random GIS records
    for (int i = 0; i < 1000000; i++)
    {
        GISRecord record = generateRecord();

        // Output the ID, first optional field, latitude, and longitude in DMS format
        file << record.id << "|" << record.optional_fields[0] << "|" << record.optional_fields[1] << "|" << record.optional_fields[2] << "|" << std::string(3, '|')
             << s(record.latitude_degrees) << s(record.latitude_minutes)
             << s(record.latitude_seconds) << record.latitude_hemisphere << "|"
             << s(record.longitude_degrees, 3) << s(record.longitude_minutes)
             << s(record.longitude_seconds) << record.longitude_hemisphere;

            // Output the remaining optional fields, separated by the "|" symbol
            for (int i = 1; i < record.optional_fields.size() - 5; i++)
            {
                file << "|" << record.optional_fields[i];
            }

            file << std::endl;
    }

    // Close the file
    file.close();

    return 0;
}
