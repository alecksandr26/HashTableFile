#ifndef __SAT_H__
#define __SAT_H__

#include <stddef.h>
#include <stdint.h>

typedef struct sat_t {
    int head, tail;
    size_t size, data_size;
} Sat;

/* sat_size: Return the size of the linked list if success getting the size  return -1 otherwise  */
int sat_size(Sat *sat);

/* sat_get_tail: Return an address of the data if success getting thedata of the head
   return null address otherwise*/
const uint8_t *sat_get_tail(Sat *sat);

/* sat_get_head: Return an address of the data if success getting thedata of the head
   return null address otherwise*/
const uint8_t *sat_get_head(Sat *sat);

/* sat_get_data: Return an address of the data if success, otherwise return NULL address */
const uint8_t *sat_get_data(Sat *sat, const uint8_t *data_to_cmp,
                            int (*compare_data)(const uint8_t *data1, const uint8_t *data2));

/* sat_rem: Return "0" if success removing the data return -1 otherwise
   compare_data: Return "0" if the data are equal return -1 otherwise */
int sat_rem(Sat *sat, const uint8_t *data_to_rem,
            int (*compare_data)(const uint8_t *data1, const uint8_t *data2));

/* sat_is_in: Return 1 if the data exist in the list else return 0 return -1 otherwise
   compare_data: Return "0" if the data are equal return otherting otherwise */
int sat_is_in(Sat *sat, const uint8_t *data,
              int (*compare_data)(const uint8_t *data1, const uint8_t *data2));

/* sat_app: Return "0" if success appending a new cell to the last part of the file return -1 otherwise */
int sat_app(Sat *sat, const uint8_t *data);

/* sat_cosnt: Return "0" if success constructing a new instance return -1 otherwise */
int sat_const(Sat *sat, size_t data_size);

/* sat_dest: Return "0" if success destructing the instance return -1 otherwise */
int sat_dest(Sat *sat);

/* open_saturation_file: Return "0" if success creating or openning a new staturation file return -1 otherwise */
int open_saturation_file(const char *file_name);

/* close_saturation_file: Return "0" if success closing the saturation file return -1 otherwise */
int close_saturation_file();

#endif
