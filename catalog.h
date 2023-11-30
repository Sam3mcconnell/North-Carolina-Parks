/**
    @file catalog.h
    @author Samuel E McConnell (semcconn)
    This is the header file for catalog.c. This file lets the other components use
    the functions in catalog.c.
*/

/** The initial capacity for the catalog parks array*/
#define INITIAL_CAPACITY 5
/** The max counties a park can have */
#define MAX_COUNTIES 5
/** The max name length a county can have */
#define MAX_COUNTIES_NAME_LENGTH 12
/** The max name length a park can have */
#define MAX_NAME_LENGTH 40

/**
 * This is the struct for the park. It has 5 variable to it.
 * @param id as the id of the park
 * @param name as the name of the park
 * @param lat as the latitude of the park
 * @param lon as the longitude of the park
 * @param counties as the list of counties.
 */
typedef struct Park
{
    int id;                                                // Park ID
    char *name;                                            // Array for the park name
    double lat;                                            // Latitude
    double lon;                                            // Longitude
    char counties[MAX_COUNTIES][MAX_COUNTIES_NAME_LENGTH]; // Double array of park county names
} Park;

/**
 * This is the struct for the catalog. It has 3 variable to it.
 * @param parks as the list of parks
 * @param count as the count of parks in the catalog
 * @param capacity as the max amount of parks in the catalog.
 */
typedef struct Catalog
{
    Park **parks;
    int count;
    int capacity;
} Catalog;

/**
 * This is the struct for the trip. It has 3 variable to it.
 * @param parks as the list of parks
 * @param count as the count of parks in the trip
 * @param capacity as the max amount of parks in the trip.
 */
typedef struct Trip
{
    Park **parks;
    int count;
    int capacity;
} Trip;

/**
 * This returns the distance in miles between two parks. It computes this
 * distance based on the Parks’ global coordinates.
 * @param a as a pointer to a park.
 * @param b as a second pointer to a park that is being compared to a.
 * @return the distance park b is from park a as a double.
 */
double distance(Park const *a, Park const *b);

/**
 * This function dynamically allocates storage for the Catalog, initializes its
 * fields (to store a resizable array) and returns a pointer to the new Catalog. It’s
 * kind of like a constructor in Java.
 * @return the catalog that it constructed.
 */
Catalog *makeCatalog();

/**
    This function frees the memory used to store the given Catalog, including freeing
    space for all the Parks, freeing the resizable array of pointers and freeing space
    for the Catalog struct itself.
    @param catalog as the catalog being freed
 */
void freeCatalog(Catalog *catalog);

/**
    This function reads all the parks from a park file with the given name.
    It makes an instance of the Park struct for each one and stores a pointer to that
    Park in the resizable array in catalog.
    @param filename as the name of the file being read.
    @param catalog as the catalog the parks will be put into.
 */
void readParks(char const *filename, Catalog *catalog);

/**
    This function sorts the parks in the given catalog. It uses the qsort() function
    together with the function pointer parameter to order the parks.
    @param catalog as the catalog that will be sorted
    @param compare as the compare method that is being used
 */
void sortParks(Catalog *catalog, int (*compare)(void const *va, void const *vb));

/**
    This function prints all or some of the parks. It uses the function pointer parameter
    together with the string, str, which is passed to the function, to decide which parks to print.
    This function will be used for the list parks, list names, and list county commands.
    @param catalog as the catalog being printed.
    @param test as the helper method to help with making sure a park has the specific county
    @param str as a const pointer to the county.
 */
void listParks(Catalog *catalog, bool (*test)(Park const *park, char const *str), char const *str);