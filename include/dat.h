#ifndef __DAT_H__
#define __DAT_H__

#include <stdint.h>
#include <stddef.h>

#ifndef FILE_NAME_LEN
#define FILE_NAME_LEN 40
#endif

typedef struct {
    int fd;
    size_t num_data, size_data;
    char file_name[FILE_NAME_LEN];
    uint8_t *buff;
} Dat;

/* dat_rem: Return "0" if success removing data return -1 otherwise */
int dat_rem(Dat *dat, const size_t pos);

/* dat_get: Return an addres of the data if success gettin the data from the file
   return a null address otherwise */
const uint8_t *dat_get(Dat *dat, const size_t pos);

/* dat_append: Return "0" if success appending the dat into the file return -1 otherwise */
int dat_append(Dat *dat, const uint8_t *data_to_append);

/* dat_const: Return "0" if success opening the dat file return -1 otherwise */
int dat_const(Dat *dat, const char *file_name, size_t size_data);

/* dat_dest: Return "0" if success closing the dat file return -1 otherwise */
int dat_dest(Dat *dat);

#endif


