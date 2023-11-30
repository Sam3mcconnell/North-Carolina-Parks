/**
    @file catalog.c
    @author Samuel E McConnell (semcconn)
    The catalog component has six functions. Each function has a specific use. This component
    creates and prints the catalog. It also frees the catalog so memory does not leak from the
    program. It also sorts the parks in the catalog when needed and helps the main function get
    the distance when printing trips.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "input.h"
#include "catalog.h"

/** Multiplier for converting degrees to radians */
#define DEG_TO_RAD (M_PI / 180)

/** Radius of the earth in miles. */
#define EARTH_RADIUS 3959.0

/**
 * This returns the distance in miles between two parks. It computes this
 * distance based on the Parks’ global coordinates.
 * @param a as a pointer to a park.
 * @param b as a second pointer to a park that is being compared to a.
 * @return the distance park b is from park a as a double.
 */
double distance(Park const *a, Park const *b)
{
    double lat1 = a->lat;
    double lon1 = a->lon;
    double lat2 = b->lat;
    double lon2 = b->lon;

    double v1[] = {cos(lon1 * DEG_TO_RAD) * cos(lat1 * DEG_TO_RAD),
                   sin(lon1 * DEG_TO_RAD) * cos(lat1 * DEG_TO_RAD),
                   sin(lat1 * DEG_TO_RAD)};

    double v2[] = {cos(lon2 * DEG_TO_RAD) * cos(lat2 * DEG_TO_RAD),
                   sin(lon2 * DEG_TO_RAD) * cos(lat2 * DEG_TO_RAD),
                   sin(lat2 * DEG_TO_RAD)};

    double dp = 0.0;
    for (int i = 0; i < sizeof(v1) / sizeof(v1[0]); i++)
    {
        dp += v1[i] * v2[i];
    }
    if (dp > 1)
    {
        return 0;
    }
    double angle = acos(dp);
    return EARTH_RADIUS * angle;
}
/**
 * This function dynamically allocates storage for the Catalog, initializes its
 * fields (to store a resizable array) and returns a pointer to the new Catalog. It’s
 * kind of like a constructor in Java.
 * @return the catalog that it constructed.
 */
Catalog *makeCatalog()
{
    Catalog *catalog = (Catalog *)malloc(sizeof(Catalog));
    catalog->parks = (Park **)malloc(sizeof(Park *) * INITIAL_CAPACITY);
    catalog->count = 0;
    catalog->capacity = INITIAL_CAPACITY;
    return catalog;
}

/**
    This function frees the memory used to store the given Catalog, including freeing
    space for all the Parks, freeing the resizable array of pointers and freeing space
    for the Catalog struct itself.
    @param catalog as the catalog being freed
 */
void freeCatalog(Catalog *catalog)
{
    for (int i = 0; i < catalog->count; i++)
    {
        if (catalog->parks[i] != NULL)
        {
            free(catalog->parks[i]->name);
            free(catalog->parks[i]);
            catalog->parks[i] = NULL;
        }
    }
    free(catalog->parks);
    free(catalog);
}

/**
    This function reads all the parks from a park file with the given name.
    It makes an instance of the Park struct for each one and stores a pointer to that
    Park in the resizable array in catalog.
    @param filename as the name of the file being read.
    @param catalog as the catalog the parks will be put into.
 */
void readParks(char const *filename, Catalog *catalog)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Can't open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char *line;
    while ((line = readLine(fp)) != NULL)
    {

        if (catalog->count == catalog->capacity)
        {
            catalog->capacity *= 2;
            Park **newParks = realloc(catalog->parks, sizeof(Park *) * catalog->capacity);
            if (!newParks)
            {
                fprintf(stderr, "Invalid park file: %s\n", filename);
                exit(EXIT_FAILURE);
            }
            catalog->parks = newParks;
        }

        Park *park = (Park *)malloc(sizeof(Park));
        int id;
        double longitude, latitude;
        if (sscanf(line, "%d %lf %lf", &id, &latitude, &longitude) != 3)
        {
            fprintf(stderr, "Invalid park file: %s\n", filename);
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < catalog->count; i++)
        {
            if (catalog->parks[i]->id == id)
            {
                fprintf(stderr, "Invalid park file: %s\n", filename);
                exit(EXIT_FAILURE);
            }
        }

        park->id = id;
        park->lat = latitude;
        park->lon = longitude;

        int spaceCount = 0;
        int i = 0;
        while (spaceCount < 3)
        {
            if (line[i] == ' ')
            {
                spaceCount++;
            }
            i++;
        }

        int length = strlen(line) - i;
        char *counties = (char *)malloc(length + 1);
        strncpy(counties, line + i, length);
        counties[length] = '\0';
        length = strlen(counties);

        int j = 0;
        int countyCount = 0;
        int countyNameLength = 0;
        char eachCounty[MAX_COUNTIES_NAME_LENGTH + 1];
        while (j <= length)
        {
            if (counties[j] == ' ' || counties[j] == '\0')
            {
                eachCounty[countyNameLength] = '\0';
                if (countyNameLength > MAX_COUNTIES_NAME_LENGTH)
                {
                    fprintf(stderr, "Invalid park file: %s\n", filename);
                    exit(EXIT_FAILURE);
                }
                strcpy(park->counties[countyCount], eachCounty);
                for (int x = 0; x < countyNameLength; x++)
                {
                    eachCounty[x] = '\0';
                }

                countyCount++;
                if (countyCount > MAX_COUNTIES)
                {
                    fprintf(stderr, "Invalid park file: %s\n", filename);
                    exit(EXIT_FAILURE);
                }

                if (counties[j] == '\0' || counties[j] == '\n')
                {
                    strcpy(park->counties[countyCount], "\0");
                    break;
                }
                j++;
                countyNameLength = 0;
            }
            eachCounty[countyNameLength] = counties[j];
            j++;
            countyNameLength++;
        }

        park->name = readLine(fp);
        free(line);
        free(counties);
        if (strlen(park->name) > MAX_NAME_LENGTH)
        {
            fprintf(stderr, "Invalid park file: %s\n", filename);
            exit(EXIT_FAILURE);
        }
        catalog->parks[catalog->count] = park;
        catalog->count++;
    }
    fclose(fp);
}

/**
    This function sorts the parks in the given catalog. It uses the qsort() function
    together with the function pointer parameter to order the parks.
    @param catalog as the catalog that will be sorted
    @param compare as the compare method that is being used
 */
void sortParks(Catalog *catalog, int (*compare)(void const *va, void const *vb))
{
    if (catalog->count < 2)
    {
        return;
    }
    qsort(catalog->parks, catalog->count, sizeof(Park *), compare);
}

/**
    This function prints all or some of the parks. It uses the function pointer parameter
    together with the string, str, which is passed to the function, to decide which parks to print.
    This function will be used for the list parks, list names, and list county commands.
    @param catalog as the catalog being printed.
    @param test as the helper method to help with making sure a park has the specific county
    @param str as a const pointer to the county.
 */
void listParks(Catalog *catalog, bool (*test)(Park const *park, char const *str), char const *str)
{
    printf("%-3s %-40s %8s %8s Counties\n", "ID", "Name", "Lat", "Lon");

    for (int i = 0; i < catalog->count; i++)
    {
        Park *park = catalog->parks[i];

        // Check if the park matches the test function
        if (str == NULL || test(park, str))
        {
            printf("%-3d %-40s %8.3f %8.3f", park->id, park->name, park->lat, park->lon);

            // Print counties
            printf(" ");
            for (int j = 0; j < MAX_COUNTIES; j++)
            {
                if (park->counties[j][0] == '\0')
                {
                    break;
                }
                printf("%s", park->counties[j]);
                if (park->counties[j + 1][0] != '\0')
                {
                    printf(",");
                }
            }
            printf("\n");
        }
    }
}