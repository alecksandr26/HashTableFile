#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "../include/map.h"

/* The files names that we are going to need */
#define MAP_FILE_NAME "map.bin"
#define SAT_FILE_NAME "sat.bin"
#define DAT_FILE_NAME "dat.bin"

/* The hash function */

int hash(const int *num)
{
    return *num;
}


/* TODOOOOO: Rewrite the tests */


/* TESTCASES */

void TESTCASE_ins_get_consults()
{
    Map *map;
    
    char map_file_name[FILE_NAME_LEN];
    char sat_file_name[FILE_NAME_LEN];
    char dat_file_name[FILE_NAME_LEN];

    const int num1 = 1;
    const int num2 = 2;

    /* Copy the names */
    strcpy(map_file_name, MAP_FILE_NAME);
    strcpy(dat_file_name, DAT_FILE_NAME);
    strcpy(sat_file_name, SAT_FILE_NAME);

    map = alloc_map();

    map_const(map, sizeof(int), sizeof(int), map_file_name, dat_file_name, sat_file_name, 10,
              (int (*)(const void *)) &hash);

    map_ins(map, &num1, &num1);
    map_ins(map, &num2, &num2);

    assert(*((int *) map_get_data(map, &num1)) == num1);
    assert(*((int *) map_get_data(map, &num2)) == num2);
    
    map_dest(map);
    dealloc_map(map);
}

void TESTCASE_const_dest()
{
    Map *map;
    
    char map_file_name[FILE_NAME_LEN];
    char sat_file_name[FILE_NAME_LEN];
    char dat_file_name[FILE_NAME_LEN];

    /* Copy the names */
    strcpy(map_file_name, MAP_FILE_NAME);
    strcpy(dat_file_name, DAT_FILE_NAME);
    strcpy(sat_file_name, SAT_FILE_NAME);

    map = alloc_map();

    assert(map != NULL);
    assert(map_const(map, sizeof(int), sizeof(int), map_file_name, dat_file_name, sat_file_name, 10,
                     (int (*)(const void *)) &hash) == 0);

    assert(map_dest(map) == 0);

    assert(dealloc_map(map) == 0);
    
}

void TESTCASE_alloc_dealloc()
{
    Map *map;

    map = alloc_map();

    assert(map != NULL);

    assert(dealloc_map(map) == 0);
}


int main()
{
    puts("Testing: map.o");
    remove(MAP_FILE_NAME);
    remove(SAT_FILE_NAME);
    remove(DAT_FILE_NAME);
    
    TESTCASE_alloc_dealloc();
    remove(MAP_FILE_NAME);
    remove(SAT_FILE_NAME);
    remove(DAT_FILE_NAME);

    TESTCASE_const_dest();
    remove(MAP_FILE_NAME);
    remove(SAT_FILE_NAME);
    remove(DAT_FILE_NAME);

    TESTCASE_ins_get_consults();
    remove(MAP_FILE_NAME);
    remove(SAT_FILE_NAME);
    remove(DAT_FILE_NAME);
    
    
    return 0;
}


