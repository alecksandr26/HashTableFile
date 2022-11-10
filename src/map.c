#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <fcntl.h>
#include <unistd.h>

/* Include the components */
#include "../include/sat.h"
#include "../include/dat.h"
#include "../include/map.h"

#ifndef FILE_NAME_LEN
#define FILE_NAME_LEN 40
#endif

/* This is going to be something temporal */
#define TEMP_MAP_FILE "temp_map.bin"
#define TEMP_SAT_FILE "temp_sat.bin"

#define INIT_MAX_NUM_CELLS 10

#undef Map
typedef struct map_t {
    size_t num_keys, max_num_cells, data_size, key_size;
    int fd;
    char file_name[FILE_NAME_LEN];
    
    /* Create the objects */
    Dat dat;
    int (*hash)(const void *key);
} Map;

#define __map_package_size(map) ((map)->key_size + sizeof(size_t))

/* TODOOOO: Make it dynamic it should scale taking more and more space */


/* __map_write_headers: Return "0" if success wwritting the headers return -1 otherwise */
static int __map_write_headers(Map *map)
{
    assert(map != NULL && "map shouldn't be null address");
    
    if (write(map->fd, &map->num_keys, sizeof(size_t)) != sizeof(size_t))
        return -1;
    
    if (write(map->fd, &map->max_num_cells, sizeof(size_t)) != sizeof(size_t))
        return -1;

    if (write(map->fd, &map->data_size, sizeof(size_t)) != sizeof(size_t))
        return -1;

    if (write(map->fd, &map->key_size, sizeof(size_t)) != sizeof(size_t))
        return -1;
    
    return 0;
}

/* __map_read_headers: Return "0" if success reading the headers return -1 otherwise */
static int __map_read_headers(Map *map)
{
    assert(map != NULL && "map shouldn't be null address");
    
    if (read(map->fd, &map->num_keys, sizeof(size_t)) != sizeof(size_t))
        return -1;
    
    if (read(map->fd, &map->max_num_cells, sizeof(size_t)) != sizeof(size_t))
        return -1;

    if (read(map->fd, &map->data_size, sizeof(size_t)) != sizeof(size_t))
        return -1;
    
    if (read(map->fd, &map->key_size, sizeof(size_t)) != sizeof(size_t))
        return -1;
    
    return 0;
}

/* __map_compare_package: Return "0" if the key from the package and the recived key are equal,
   otherwise return -1 */
static int __map_compare_package(const uint8_t *key, const uint8_t *package)
{
    assert(key != NULL && package != NULL && "Key and Map shouldn't be Null");

    /* TODOOOO: Change this shitty function */
    if (strcmp((const char *) key, (const char *) package + sizeof(size_t)) != 0)
        return -1;

    return 0;
}

/* __map_alloc_space: Return "0" if success allocing new space in the file return -1 otherwise */
static int __map_alloc_space(Map *map)
{
    assert(map != NULL && "map shouldn't be null address");
    size_t i;
    Sat sat;

    memset(&sat, 0, sizeof(Sat));

    if (map->fd != -1 && close(map->fd) == -1)
        return -1;

    /* Re open the file */
    if ((map->fd = open(map->file_name, O_WRONLY | O_APPEND)) == -1)
        return -1;

    /* Alloc space for the headers */
    if (__map_write_headers(map) == -1)
        return -1;

    /* Alloc the table */
    for (i = 0; i < map->max_num_cells; i++)
        if (write(map->fd, &sat, sizeof(Sat)) != sizeof(Sat))
            return -1;
    
    /* Reopen the file again */
    if (close(map->fd) == -1)
        return -1;
    
    if ((map->fd = open(map->file_name, O_RDWR)) == -1)
        return -1;
    
    return 0;
}

/* __map_open_create: Return "0" if success creating or opening the file map return -1 otherwise */
static int __map_open_create(Map *map)
{
    assert(map != NULL && "map should'nt be null address");

    if (access(map->file_name, R_OK) == 0) {
        if ((map->fd = open(map->file_name, O_RDWR)) == -1)
            return -1;

        if (__map_read_headers(map) == -1) {
            close(map->fd);
            return -1;
        }
    } else {
        if (creat(map->file_name, 0644) == -1)
            return -1;
        map->fd = -1;
        if (__map_alloc_space(map) == -1) {
            close(map->fd);
            return -1;
        }
    }
    
    return 0;
}


/* map_rep: Return "0" if success replacing the data tracked by the key return -1 otherwise */
int map_rep(Map *map, const void *key, const void *new_data)
{
    uint8_t *package;
    Sat sat, sat_zeros;
    size_t index, pos;
    
    if (map == NULL || key == NULL || new_data == NULL)
        return -1;
    
    memset(&sat_zeros, 0, sizeof(Sat));
    
    /* Map the index and fetch the structure */
    index = map->hash(key) % map->max_num_cells;


    if (lseek(map->fd, sizeof(Sat) * index + sizeof(size_t) * 4, SEEK_SET) == -1)
        return -1;

    if (read(map->fd, &sat, sizeof(Sat)) != sizeof(Sat))
         return -1;

    /* Check if the cell exist if is zero, it doesn't exist */
    if (memcmp(&sat_zeros, &sat, sizeof(Sat)) == 0)
        return -1;

     /* Fetch the package by the buffer gived by sat component */
    if ((package = (uint8_t *) sat_get_data(&sat, key,
                                            (int (*)(const uint8_t *, const uint8_t *))
                                            &__map_compare_package)) == NULL)
        return -1;


    /* Get the position of data */
    memcpy(&pos, package, sizeof(size_t));

    if (dat_rep(&map->dat, pos, new_data) == -1)
        return -1;

    return 0;
}   
    
/* map_rem: Return "0" if success removing or deleting data return -1 otherwise */
int map_rem(Map *map, const void *key)
{
    uint8_t *package;
    Sat sat, sat_zeros;
    size_t index, pos;
    
    if (map == NULL || key == NULL)
        return -1;

    memset(&sat_zeros, 0, sizeof(Sat));
    
    /* Map the index and fetch the structure */
    index = map->hash(key) % map->max_num_cells;

    if (lseek(map->fd, sizeof(Sat) * index + sizeof(size_t) * 4, SEEK_SET) == -1)
        return -1;

    if (read(map->fd, &sat, sizeof(Sat)) != sizeof(Sat))
         return -1;

    /* Check if the cell exist if is zero, it doesn't exist */
    if (memcmp(&sat_zeros, &sat, sizeof(Sat)) == 0)
        return -1;

     /* Fetch the package by the buffer gived by sat component */
    if ((package = (uint8_t *) sat_get_data(&sat, key,
                                            (int (*)(const uint8_t *, const uint8_t *))
                                            &__map_compare_package)) == NULL)
        return -1;

    /* Get the position of data */
    memcpy(&pos, package, sizeof(size_t));

    /* Delete the data from the data file*/
    if (dat_rem(&map->dat, pos) == -1)
        return -1;

    /* Remove the rell */
    if (sat_rem(&sat, key, &__map_compare_package) == -1)
        return -1;

    /* Save the new state of the cell */
    if (sat.size == 0)
        memcpy(&sat, &sat_zeros, sizeof(Sat));
        
    if (lseek(map->fd, sizeof(Sat) * index + sizeof(size_t) * 4, SEEK_SET) == -1)
        return -1;
        
    if (write(map->fd, &sat, sizeof(Sat)) != sizeof(Sat))
        return -1;
    
    return 0;
}


/* map_get: Return an address of the data if success return a null address otheriwse */
const void *map_get(Map *map, const void *key)
{
    uint8_t *package, *data;
    size_t pos, index;
    Sat sat, sat_zeros;
    
    if (map == NULL || key == NULL)
        return NULL;

    memset(&sat_zeros, 0, sizeof(Sat));
    
    /* Map the index and fetch the structure */
    index = map->hash(key) % map->max_num_cells;

    if (lseek(map->fd, sizeof(Sat) * index + sizeof(size_t) * 4, SEEK_SET) == -1)
        return NULL;

    if (read(map->fd, &sat, sizeof(Sat)) != sizeof(Sat))
         return NULL;

    /* Check if the cell exist if is zero, it doesn't exist */
    if (memcmp(&sat_zeros, &sat, sizeof(Sat)) == 0)
        return NULL;

    /* Alloc memory for the buffer */
    if (sat_set_data_size(&sat, __map_package_size(map)) == -1)
        return NULL;

    /* TODOOOOO: Refactor this shitty if statement */
    
    /* Fetch the package by the buffer gived by sat component */
    if ((package = (uint8_t *) sat_get_data(&sat, key,
                                            (int (*)(const uint8_t *, const uint8_t *))
                                            &__map_compare_package)) == NULL)
        return NULL;

    /* Get the position of data */
    memcpy(&pos, package, sizeof(size_t));

    /* Get the data from the file */
    if ((data = (uint8_t *) dat_get(&map->dat, pos)) == NULL)
        return NULL;
    
    return data;
}

/* map_ins: Return "0" if success inserting data in the map return -1 otherwise */
int map_ins(Map *map, const void *key, const void *data)
{
    size_t index;
    Sat sat_zeros, sat;
    uint8_t package[__map_package_size(map)];
    
    if (map == NULL || data == NULL || key == NULL)
        return -1;

    /* Get the index */
    index = map->hash(key) % map->max_num_cells;

    memset(&sat_zeros, 0, sizeof(Sat));

    /* Prepare the package */
    memcpy(package, &map->dat.num_data, sizeof(size_t));
    memcpy(package + sizeof(size_t), key, map->key_size);

    if (lseek(map->fd, sizeof(Sat) * index + sizeof(size_t) * 4, SEEK_SET) == -1)
        return -1;

    if (read(map->fd, &sat, sizeof(Sat)) != sizeof(Sat))
        return -1;
    
    if (memcmp(&sat_zeros, &sat, sizeof(Sat)) == 0)    /* This cell is empty */
        if (sat_const(&sat, __map_package_size(map)) == -1)
            return -1;
    
    if (sat_app(&sat, package) == -1)
        return -1;

    /* Rewrite the cell */
    if (lseek(map->fd, sizeof(Sat) * index + sizeof(size_t) * 4, SEEK_SET) == -1)
        return -1;

    if (write(map->fd, &sat, sizeof(Sat)) != sizeof(Sat))
        return -1;

    /* Finally write the data in the file */
    if (dat_append(&map->dat, data) == -1)
        return -1;
        
    return 0;
}

/* map_const: Return "0" if success initializing the files for the mapping return -1 otherwise */
int map_const(Map *map, size_t data_size, size_t key_size,
              const char file_name[FILE_NAME_LEN],
              const char dat_file_name[FILE_NAME_LEN],
              const char sat_file_name[FILE_NAME_LEN],
              const size_t max_num_cells,
              int (*hash)(const void *key))
{
    if (map == NULL || hash == NULL)
        return -1;
    
    /* Copy the files name */
    memcpy(map->file_name, file_name, FILE_NAME_LEN);

    /* Open the saturation file */
    if (open_saturation_file(sat_file_name) == -1)
        return -1;

    if (dat_const(&map->dat, dat_file_name, data_size) == -1)
        return -1;
    
    map->data_size = data_size;
    map->max_num_cells = (max_num_cells > INIT_MAX_NUM_CELLS) ? max_num_cells : INIT_MAX_NUM_CELLS;
    map->hash = hash;
    map->num_keys = 0;
    map->key_size = key_size;
    
    /* Open the file */
    if (__map_open_create(map) == -1)
        return -1;
    
    return 0;
}

/* map_dest: Return "0" if success destructing an instance of map return -1 otherwise */
int map_dest(Map *map)
{
    if (map == NULL)
        return -1;

    /* Close the file of dat object */
    if (dat_dest(&map->dat) == -1)
        return -1;
    
    /* Close the map file */
    if (close(map->fd) == -1)
        return -1;

    /* Close the file */
    if (close_saturation_file() == -1)
        return -1;
    
    return 0;
}

/* alloc_map: Return a new instance of map if success allocating return NULL address otherwise */
Map *alloc_map()
{
    Map *new_map;

    if ((new_map = (Map *) malloc(sizeof(Map))) == NULL)
        return NULL;

    return new_map;
}

/* dealloc_map: Return "0" if success freeing a instance of map return -1 otherwise */
int dealloc_map(Map *map)
{
    if (map == NULL)
        return -1;
    
    free(map);
    
    return 0;
}
  
