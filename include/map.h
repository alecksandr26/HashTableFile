#ifndef __MAP_H__
#define __MAP_H__

#include <stddef.h>

#ifndef FILE_NAME_LEN
#define FILE_NAME_LEN 40
#endif

/* Map header structure */
typedef struct map_t Map;

/* map_get_data: Return an address of the data if success return a null address otheriwse */
const void *map_get_data(Map *map, const void *key);

/* map_ins: Return "0" if success inserting data in the map return -1 otherwise */
int map_ins(Map *map, const void *data, const void *key);

/* map_const: Return "0" if success initializing the files for the mapping return -1 otherwise */
int map_const(Map *map, size_t ele_size, size_t key_size,
              const char map_file_name[FILE_NAME_LEN],
              const char dat_file_name[FILE_NAME_LEN],
              const char sat_file_name[FILE_NAME_LEN],
              const size_t max_num_cells,
              int (*hash)(const void *key));

/* map_dest: Return "0" if success destructing an instance of map return -1 otherwise */
int map_dest(Map *map);

/* alloc_map: Return a new instance of map if success allocating return NULL address otherwise */
Map *alloc_map();

/* dealloc_map: Return "0" if success freeing a instance of map return -1 otherwise */
int dealloc_map(Map *map);

#endif
