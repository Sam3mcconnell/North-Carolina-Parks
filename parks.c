/**
    @file catalog.c
    @author Samuel E McConnell (semcconn)
    The parks component has 10 functions including the main function. The main function runs
    the program and calls all the needed functions to run the program correctly. The other functions
    help make the trip aspect of the program and help sort and print the parks.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "input.h"
#include "catalog.h"

/** The max line length of each line to be read from input */
#define MAX_LINE_LENGTH 256

/**
 * This function dynamically allocates storage for the Trip, initializes its
 * fields (to store a resizable array) and returns a pointer to the new Trip. It’s
 * kind of like a constructor in Java.
 * @return the trip that it constructed.
 */
static Trip *makeTrip()
{
    Trip *trip = (Trip *)malloc(sizeof(Trip));
    trip->parks = (Park **)malloc(sizeof(Park *) * INITIAL_CAPACITY);
    trip->count = 0;
    trip->capacity = INITIAL_CAPACITY;
    return trip;
}

/**
    This function frees the memory used to store the given Trip, including freeing
    space for all the Parks, freeing the resizable array of pointers and freeing space
    for the Trip struct itself.
    @param trip as the trip being freed
 */
static void freeTrip(Trip *trip)
{

    for (int i = 0; i < trip->count; i++)
    {
        if (trip->parks[i] != NULL)
        {
            trip->parks[i] = NULL;
        }
    }
    free(trip->parks);
    free(trip);
}

/**
 * This function adds a park to the trip. It makes sure that the park exists in the catalog
 * and also that the park has not already been added.
 * @param catalog as the catalog
 * @param trip as the trip
 * @param id as the park's id that is being added
 */
static void addParkToTrip(Catalog *catalog, Trip *trip, int id)
{
    if (trip->count == trip->capacity)
    {
        trip->capacity *= 2;
        Park **newParks = realloc(trip->parks, sizeof(Park *) * trip->capacity);
        if (!newParks)
        {
            printf("Error: realloc failed\n");
            exit(1);
        }
        trip->parks = newParks;
    }

    bool check = false;
    for (int i = 0; i < catalog->count; i++)
    {
        if (catalog->parks[i]->id == id)
        {
            check = true;
            trip->parks[trip->count] = catalog->parks[i];
            trip->count++;
        }
    }
    if (!check)
    {
        printf("Invalid command\n");
    }
}

/**
 * This function removes a park from the trip. It makes sure that the park exists in the trip.
 * @param catalog as the catalog
 * @param trip as the trip
 * @param id as the park's id that is being removed.
 */
static void removeParkFromTrip(Trip *trip, int parkID)
{
    int index = -1;
    for (int i = 0; i < trip->count; i++)
    {
        if (trip->parks[i]->id == parkID)
        {
            index = i;
            break;
        }
    }

    if (index != -1)
    {
        trip->parks[index] = NULL;
        for (int i = index; i < trip->count - 1; i++)
        {
            trip->parks[i] = trip->parks[i + 1];
        }
        trip->count--;
    }
    else
    {
        printf("Invalid command\n");
    }
}

/**
    This function prints all of the trip information. It uses the distance function to help
    calculate the distance from the first park that was added to the trip.
    @param trip as the trip being printed.
 */
static void listTrip(Trip *trip)
{
    printf("%-3s %-40s %8s\n", "ID", "Name", "Distance");
    double totalDistance = 0.0;
    Park *previousPark = NULL;
    for (int i = 0; i < trip->count; i++)
    {
        Park *park = trip->parks[i];
        double dist = (previousPark != NULL) ? distance(previousPark, park) : 0.0;
        totalDistance += dist;
        printf("%-3d %-40s %8.1f\n", park->id, park->name, totalDistance);
        previousPark = park;
    }
}

/**
    This function finds the nearest parks from the last park that you added to the trip.
    It will sort through the catalog and find the amount of trips that you requested and prints
    them.
    @param catalog as the catalog
    @param trip as the trip
    @param amount as the amount of parks you would like to be listed.
 */
static void getNearest(Catalog *catalog, Trip *trip, int amount)
{
    if (amount <= 0 || trip->count == 0)
    {
        printf("Invalid command\n");
        return;
    }

    if (amount > catalog->count)
    {
        amount = catalog->count - 1;
    }

    Park **nearestList = (Park **)malloc(sizeof(Park) * (amount + 1));
    nearestList[0] = trip->parks[trip->count - 1];
    for (int i = 1; i <= amount; i++)
    {
        for (int j = 0; j < catalog->count; j++)
        {
            if (j == 0)
            {
                for (int x = 0; x <= amount; x++)
                {
                    if (nearestList[x] == NULL)
                    {
                        break;
                    }
                    if (catalog->parks[j]->id == nearestList[x]->id)
                    {
                        j++;
                        x = 0;
                    }
                }
                nearestList[i] = catalog->parks[j];
            }

            bool isInNearestList = false;
            for (int y = 0; y < i; y++)
            {
                if (nearestList[y] == NULL)
                {
                    break;
                }
                if (catalog->parks[j]->id + 1 == nearestList[y]->id + 1)
                {
                    isInNearestList = true;
                }
            }

            if (distance(nearestList[0], catalog->parks[j]) < distance(nearestList[0], nearestList[i]) && !isInNearestList)
            {
                nearestList[i] = catalog->parks[j];
            }
        }
    }

    printf("%-3s %-40s %8s\n", "ID", "Name", "Distance");
    for (int i = 0; i <= amount; i++)
    {
        Park *park = nearestList[i];
        double dist = distance(nearestList[0], park);
        printf("%-3d %-40s %8.1f\n", park->id, park->name, dist);
    }

    free(nearestList);
}

/**
    This function compares the parks by id. This is used when we are sorting the parks by
    id in the catalog. It returns either 0 if the parks are equal, 1 if the park a is greater
    than park b, or -1 if park b is greater than park a.
    @param a as a park being compared
    @param b as a park being compared
    @return an int value to sort.
 */
static int compareParksByID(const void *a, const void *b)
{
    Park *parkA = *(Park **)a;
    Park *parkB = *(Park **)b;

    return (parkA->id < parkB->id) ? -1 : (parkA->id > parkB->id);
}

/**
    This function compares the parks by name. This is used when we are sorting the parks by
    name in the catalog. It returns either 0 if the parks are equal, 1 if the park a is greater
    than park b, or -1 if park b is greater than park a.
    @param a as a park being compared
    @param b as a park being compared
    @return an int value to sort.
 */
static int compareParksByName(const void *a, const void *b)
{
    Park *parkA = *(Park **)a;
    Park *parkB = *(Park **)b;

    int nameCompare = strcmp(parkA->name, parkB->name);

    if (nameCompare != 0)
    {
        return nameCompare;
    }
    else
    {
        return (parkA->id < parkB->id) ? -1 : (parkA->id > parkB->id);
    }
}

/**
    This function is used to check if a park has the given county. It is used by the print parks
    when the user wishes to see all the parks in a county.
    @param park as the park that might have the county
    @param str is the county.
    @return true if the park is in the county, false if not.
 */
static bool countyTestFunction(Park const *park, char const *str)
{
    for (int i = 0; i < MAX_COUNTIES; i++)
    {
        if (park->counties[i][0] == '\0')
        {
            break;
        }
        if (strcmp(park->counties[i], str) == 0)
        {
            return true;
        }
    }
    return false;
}

/**
    This is the main function of the program. It will start the prgram and call all the required
    functions to make the program run correctly. This function also takes on the files it needs to read.
    @param argc as the amount of arguments.
    @param agrv as the list of arguments.
    @return EXIT_SUCCESS or EXIT_FALURE.
 */
int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "usage: parks <park-file>*\n");
        return EXIT_FAILURE;
    }

    Catalog *catalog = makeCatalog();

    Trip *trip = makeTrip();

    for (int i = 1; i < argc; i++)
    {
        readParks(argv[i], catalog);
    }

    char input[MAX_LINE_LENGTH];
    while (1)
    {
        printf("cmd> ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        size_t input_length = strlen(input);
        if (input_length > 0 && input[input_length - 1] == '\n')
        {
            input[input_length - 1] = '\0';
        }

        char cmd[MAX_LINE_LENGTH];
        char param1[MAX_LINE_LENGTH];
        char param2[MAX_LINE_LENGTH];
        int result = sscanf(input, "%s %s %s", cmd, param1, param2);

        if (result == 1 && strcmp(cmd, "quit") == 0)
        {
            printf("%s\n", input);
            break;
        }
        else if (result >= 1 && strcmp(cmd, "list") == 0)
        {
            if (strcmp(param1, "parks") == 0)
            {
                printf("%s\n", input);
                sortParks(catalog, compareParksByID);
                listParks(catalog, countyTestFunction, NULL);
            }
            else if (strcmp(param1, "names") == 0)
            {
                printf("%s\n", input);
                sortParks(catalog, compareParksByName);
                listParks(catalog, countyTestFunction, NULL);
            }
            else if (strcmp(param1, "county") == 0)
            {
                printf("%s\n", input);
                listParks(catalog, countyTestFunction, param2);
            }
            else
            {
                printf("%s\n", input);
                printf("Invalid command\n");
            }
        }
        else if (result >= 1 && strcmp(cmd, "add") == 0)
        {
            printf("%s\n", input);
            addParkToTrip(catalog, trip, atoi(param1));
        }
        else if (result >= 1 && strcmp(cmd, "remove") == 0)
        {
            printf("%s\n", input);
            removeParkFromTrip(trip, atoi(param1));
        }
        else if (result >= 1 && strcmp(cmd, "trip") == 0)
        {
            printf("%s\n", input);
            listTrip(trip);
        }
        else if (result >= 1 && strcmp(cmd, "nearest") == 0)
        {
            printf("%s\n", input);
            getNearest(catalog, trip, atoi(param1));
        }
        else
        {
            printf("%s\n", input);
            printf("Invalid command\n");
        }
        printf("\n");
    }
    freeCatalog(catalog);
    freeTrip(trip);

    return EXIT_SUCCESS;
}