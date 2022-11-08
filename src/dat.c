#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <fcntl.h>
#include <unistd.h>

#include "../include/dat.h"

/* Implement a system to catch the deleted register and save space */

/* __dat_read_headers: Return "0" if success reading the headers return -1 otherwise */
static int __dat_read_headers(Dat *dat)
{
    assert(dat != NULL && "dat shouldn't be null");
    
    if (lseek(dat->fd, 0, SEEK_SET) == -1)
        return -1;

    if (read(dat->fd, &dat->num_data, sizeof(size_t)) != sizeof(size_t))
        return -1;
    
    if (read(dat->fd, &dat->data_size, sizeof(size_t)) != sizeof(size_t))
        return -1;
    
    return 0;
}

/* __dat_write_headers: Return "0" if success writting the headers return -1 otherwise */
static int __dat_write_headers(Dat *dat)
{
    assert(dat != NULL && "dat shouldn't be null");

    if (lseek(dat->fd, 0, SEEK_SET) == -1)
        return -1;
    
    if (write(dat->fd, &dat->num_data, sizeof(size_t)) != sizeof(size_t))
        return -1;

    if (write(dat->fd, &dat->data_size, sizeof(size_t)) != sizeof(size_t))
        return -1;
    
    return 0;
}

/* dat_rep: Return "0" if success replacing the data return -1 otherwise */
int dat_rep(Dat *dat, size_t pos, const uint8_t *new_data)
{
    if (dat == NULL || new_data == NULL || pos >= dat->num_data)
        return -1;

    if (lseek(dat->fd, dat->data_size * pos + sizeof(size_t) * 2, SEEK_SET) == -1)
        return -1;

    if (write(dat->fd, new_data, dat->data_size) != (int) dat->data_size)
        return -1;
    
    return 0;
}

/* dat_rem: Return "0" if success removing data return -1 otherwise */
int dat_rem(Dat *dat, const size_t pos)
{
    uint8_t buff[dat->data_size];
    
    if (dat == NULL || pos >= dat->num_data)
        return -1;

    if (lseek(dat->fd, dat->data_size * pos + sizeof(size_t) * 2, SEEK_SET) == -1)
        return -1;

    memset(buff, 'd', dat->data_size);
    if (write(dat->fd, buff, dat->data_size) != (int) dat->data_size)
        return -1;
    
    if (__dat_write_headers(dat) == -1) {
        close(dat->fd);
        return -1;
    }

    return 0;
}


/* dat_get: Return an addres of the data if success gettin the data from the file
   return a null address otherwise */
const uint8_t *dat_get(Dat *dat, const size_t pos)
{
    if (dat == NULL || pos >= dat->num_data)
        return NULL;

    if (lseek(dat->fd, dat->data_size * pos + sizeof(size_t) * 2, SEEK_SET) == -1)
        return NULL;
    
    if (read(dat->fd, dat->buff, dat->data_size) != (int) dat->data_size)
        return NULL;
    
    return dat->buff;
}


/* dat_append: Return "0" if success appending the dat into the file return -1 otherwise */
int dat_append(Dat *dat, const uint8_t *data_to_append)
{
    if (dat == NULL || data_to_append == NULL)
        return -1;

    if (close(dat->fd) == -1)
        return -1;

    if ((dat->fd = open(dat->file_name, O_RDWR | O_APPEND)) == -1)
        return -1;

    /* Lets write */
    if (write(dat->fd, data_to_append, dat->data_size) != (int) dat->data_size)
        return -1;

    if (close(dat->fd) == -1)
        return -1;

    if ((dat->fd = open(dat->file_name, O_RDWR)) == -1)
        return -1;
    
    dat->num_data++;

    if (__dat_write_headers(dat) == -1) {
        close(dat->fd);
        return -1;
    }

    return 0;
}

/* dat_const: Return "0" if success opening the dat file return -1 otherwise */
int dat_const(Dat *dat, const char *file_name, size_t data_size)
{
    /* Prepare the dat */
    memcpy(dat->file_name, file_name, FILE_NAME_LEN);
    dat->num_data = 0;
    dat->data_size = data_size;

    if (access(file_name, R_OK) == 0) {
        if ((dat->fd = open(file_name, O_RDWR | O_APPEND)) == -1)
            return -1;
        if (__dat_read_headers(dat) == -1) {
            close(dat->fd);
            return -1;
        }
    } else {
        if (creat(file_name, 0644) == -1)
            return -1;
        
        if ((dat->fd = open(file_name, O_RDWR | O_APPEND)) == -1)
            return -1;

        if (__dat_write_headers(dat) == -1) {
            close(dat->fd);
            return -1;
        }
    }

    if (close(dat->fd) == -1)
        return -1;
    
    if ((dat->fd = open(file_name, O_RDWR)) == -1)
        return -1;

    if ((dat->buff = malloc(data_size)) == NULL)
        return -1;
    
    return 0;
}

/* dat_dest: Return "0" if success closing the dat file return -1 otherwise */
int dat_dest(Dat *dat)
{

    if (close(dat->fd) == -1)
        return -1;

    if ((dat->fd = open(dat->file_name, O_RDWR)) == -1)
        return -1;

    
    if (close(dat->fd) == -1)
        return -1;

    free(dat->buff);
    memset(dat, 0, sizeof(Dat));
    
    return 0;
}
