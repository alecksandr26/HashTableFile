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


#define LEN_NAME 30
#define LEN_PASSWORD 30
#define LEN_ID 30
#define MAX_NUM_CELLS 0

/* The hash function */

/* User struct */
typedef struct {
    char name[LEN_NAME];
    char password[LEN_PASSWORD];
    char id[LEN_ID];
    int num;
} User;


int hash(const char *key)
{
    size_t i;
    int sum;

    sum = 0;
    for (i = 0; i < strlen(key); ++i)
        sum += key[i];

    return sum;
}


/* TODOOOOO: Rewrite the tests */


/* TESTCASES */


void TESTCASE_rem_rep_get_consults()
{
    Map *map;
    
    char map_file_name[FILE_NAME_LEN];
    char sat_file_name[FILE_NAME_LEN];
    char dat_file_name[FILE_NAME_LEN];

    const User user1 = {"Pedrito", "Chamito", "1234", 1};
    const User user2 = {"Julio", "Chanchito", "youlove", 2};
    const User user3 = {"Julio", "Chanchito", "youlove", 3};

    /* Copy the names */
    strcpy(map_file_name, MAP_FILE_NAME);
    strcpy(dat_file_name, DAT_FILE_NAME);
    strcpy(sat_file_name, SAT_FILE_NAME);

    map = alloc_map();

    map_const(map, sizeof(User), LEN_ID, map_file_name, dat_file_name, sat_file_name, 10,
              (int (*)(const void *)) &hash);

    map_ins(map, user1.id, &user1);
    map_ins(map, user2.id, &user2);

    assert(map_rem(map, user1.id) == 0);
    assert(map_get(map, user1.id) == NULL);

    map_ins(map, user1.id, &user1);
    
    assert(((User *) map_get(map, user1.id))->num == user1.num);

    memcpy((char *) user3.id, user1.id, LEN_ID);
    assert(map_rep(map, user1.id, &user3) == 0);
    
    assert(((User *) map_get(map, user1.id))->num == user3.num);
    
    map_dest(map);
    dealloc_map(map);
}


void TESTCASE_ins_get_consults()
{
    Map *map;
    
    char map_file_name[FILE_NAME_LEN];
    char sat_file_name[FILE_NAME_LEN];
    char dat_file_name[FILE_NAME_LEN];

    const User user1 = {"Pedrito", "Chamito", "1234", 1};
    const User user2 = {"Julio", "Chanchito", "youlove", 2};

    /* Copy the names */
    strcpy(map_file_name, MAP_FILE_NAME);
    strcpy(dat_file_name, DAT_FILE_NAME);
    strcpy(sat_file_name, SAT_FILE_NAME);

    map = alloc_map();

    map_const(map, sizeof(User), LEN_ID, map_file_name, dat_file_name, sat_file_name, 10,
              (int (*)(const void *)) &hash);

    assert(map_ins(map, user1.id, &user1) == 0);
    assert(map_ins(map, user2.id, &user2) == 0);

    assert(((User *) map_get(map, user1.id))->num == 1);
    assert(((User *) map_get(map, user2.id))->num == 2);
    
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

    TESTCASE_rem_rep_get_consults();
    remove(MAP_FILE_NAME);
    remove(SAT_FILE_NAME);
    remove(DAT_FILE_NAME);
    
    return 0;
}


