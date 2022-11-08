#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>

#include "../include/sat.h"

/* To avoid errors in the code */
#define FILE_NAME_LEN 40

/* Cell register */
typedef struct cell_t {
    int next_pos;
    size_t curernt_pos;
} Cell;

/* Global variables */
static int FD = -1;
static size_t NUM_CELLS;
static char FILE_NAME[FILE_NAME_LEN];
static uint8_t *DATA;

/* __size_cell: Return the actual size of a cell */
#define __size_cell(sat) (sizeof(Cell) + (sat)->data_size)

/* __check_next: Return true if there is another position */
#define __check_next(cell) ((cell).next_pos != -1)

/* __sat_write_headers: Return "0" if allocing the space succeeded */
static int __sat_write_headers()
{
    assert(FD > 2 && "sat->fd should'nt be lesser than 3");

    if (lseek(FD, 0, SEEK_SET) == -1)
        return -1;
    
    if (write(FD, &NUM_CELLS, sizeof(size_t)) == -1)
        return -1;

    return 0;
}

/* __sat_read_headers: Return 0 if success reading the headers return -1 */
static int __sat_read_headers()
{
    assert(FD > 2 && "sat->fd should'nt be lesser than 3");

    if (lseek(FD, 0, SEEK_SET) == -1)
        return -1;

    if (read(FD, &NUM_CELLS, sizeof(size_t)) == -1)
        return -1;
    
    return 0;
}

/* __sat_next_cell: Return an "0" if success moving to the next cell return -1 otherwise */
static int __sat_next_cell(Sat *sat, Cell *cell, uint8_t *data)
{
    assert(sat != NULL && cell != NULL && "sat should'nt be null and cell should'nt be null ");

    if (cell->next_pos == -1)
        return -1;

    if (lseek(FD, cell->next_pos * __size_cell(sat) + sizeof(size_t), SEEK_SET) == -1)
        return -1;

    /* Read the cell */
    if (read(FD, cell, sizeof(Cell)) != sizeof(Cell))
        return -1;

    if (read(FD, data, sat->data_size) != (int) sat->data_size)
        return -1;
    
    return 0;
}

/* __sat_set_next_cell: Return "0" if success linking these cells return -1 otherwise */
static int __sat_set_next_cell(Sat *sat, size_t pos, int next_pos)
{
    Cell cell;
    
    if (lseek(FD, pos * __size_cell(sat) + sizeof(size_t), SEEK_SET) == -1)
        return -1;

    if (read(FD, &cell, sizeof(Cell)) != sizeof(Cell))
        return -1;

   if (lseek(FD, pos * __size_cell(sat) + sizeof(size_t), SEEK_SET) == -1)
        return -1;

    cell.next_pos = next_pos;
    if (write(FD, &cell, sizeof(Cell)) != sizeof(Cell))
        return -1;

    return 0;
}


/* __sat_get_tail: Return "0" if success getting the tail cell return -1 otherwise */
static int __sat_get_tail(Sat *sat, Cell *cell, uint8_t *buff)
{
    assert(sat != NULL && cell != NULL && buff != NULL && "sat and cell shouldn't be null values");

    if (lseek(FD, __size_cell(sat) * sat->tail + sizeof(size_t), SEEK_SET) == -1)
        return -1;

    if (read(FD, cell, sizeof(Cell)) != sizeof(Cell))
        return -1;

    if (read(FD, buff, sat->data_size) != (int) sat->data_size)
        return -1;

    return 0;
}

/* __sat_get_head: Return "0" if success getting the head cell return -1 otherwise */
static int __sat_get_head(Sat *sat, Cell *cell, uint8_t *buff)
{
    assert(sat != NULL && cell != NULL && buff != NULL && "sat and cell shouldn't be null values");

    if (lseek(FD, __size_cell(sat) * sat->head + sizeof(size_t), SEEK_SET) == -1)
        return -1;

    if (read(FD, cell, sizeof(Cell)) != sizeof(Cell))
        return -1;

    if (read(FD, buff, sat->data_size) != (int) sat->data_size)
        return -1;

    return 0;
}

/* __sat_app_new_cell: Return "0" If success adding a new cell in the file return -1 otherwise */
static int __sat_app_new_cell(Sat *sat, Cell *new_cell, const uint8_t *data)
{
    assert(sat != NULL && new_cell != NULL && data != NULL && "sat, cell, data shouldn't be null values");
    
    /* First close the file */
    if (close(FD) == -1)
        return -1;

    /* Append a new cell */
    if ((FD = open(FILE_NAME, O_WRONLY | O_APPEND)) == -1)
        return -1;

    if (write(FD, new_cell, sizeof(Cell)) != sizeof(Cell)) {
        close(FD);
        return -1;
    }

    if (write(FD, data, sat->data_size) != (int) sat->data_size) {
        close(FD);
        return -1;
    }
    
    /* Reopen again the file */
    if (close(FD) == -1)
        return -1;
    
    if ((FD = open(FILE_NAME, O_RDWR)) == -1)
        return -1;
    
    
    return 0;
}

/* __sat_overwrite_cell: Return "0" if success overwritting the cell return -1 otherwise */
static int __sat_overwrite_cell(Sat *sat, size_t pos, Cell *update_cell)
{
    assert(sat != NULL && update_cell != NULL && "sat cell shouldn't be null values");

    if (lseek(FD, __size_cell(sat) * pos + sizeof(size_t), SEEK_SET) == -1)
        return -1;
    
    if (write(FD, update_cell, sizeof(Cell)) != sizeof(Cell))
        return -1;
    
    return 0;
}


/* TODOOOOOOO: Create a functionality that manages the deleted or removed registers */

/* __sat_del: Return "0" if success deleting a linking key in the cell */
/* 
static int __sat_del(Sat *sat, const char *key)
{
    return 0;
}
*/

/* sat_size: Return the size of the linked list if success getting the size  return -1 otherwise  */
int sat_size(Sat *sat)
{
    if (sat == NULL)
        return -1;
    
    return sat->size;
}

/* sat_set_data_size: Return "0" if success setting a size to the data return -1 otherwise */
int sat_set_data_size(Sat *sat, const size_t new_data_size)
{
    if (sat == NULL)
        return -1;

    sat->data_size = new_data_size;
    
    /* If data is equal to NULL try to alloc the necessary memory */
    if (DATA == NULL && (DATA = malloc(new_data_size)) == NULL)
        return -1;
    else if ((DATA = realloc(DATA, new_data_size)) == NULL)
        return -1;

    return 0;
}


/* sat_get_tail: Return an address of the data if success getting thedata of the head
   return null address otherwise*/
const uint8_t *sat_get_tail(Sat *sat)
{
    Cell cell;
    
    if (sat == NULL)
        return NULL;
    
    if (__sat_get_tail(sat, &cell, DATA) == -1)
        return NULL;
    
    return DATA;
}



/* sat_get_head: Return an address of the data if success getting thedata of the head
   return null address otherwise*/
const uint8_t *sat_get_head(Sat *sat)
{
    Cell cell;
    
    if (sat == NULL)
        return NULL;

    if (__sat_get_head(sat, &cell, DATA) == -1)
        return NULL;
    
    return DATA;
}

/* sat_get_data: Return an address of the data if success, otherwise return NULL address
   compare_data: Return "0" if the data are equal return -1 otherwise */
const uint8_t *sat_get_data(Sat *sat, const uint8_t *data_to_cmp,
                            int (*compare_data)(const uint8_t *data1, const uint8_t *data2))
{
    Cell cell;
    
    if (sat == NULL || data_to_cmp == NULL || compare_data == NULL)
        return NULL;

    if (__sat_get_tail(sat, &cell, DATA) == -1) {
        close(FD);
        return NULL;
    }

    do {
        if (compare_data(data_to_cmp, DATA) == 0)
            return DATA;
    } while (__sat_next_cell(sat, &cell, DATA) != -1);

    return NULL;
}

/* TODOOOOOO: Refactor in sat_rem pls */

/* sat_rem: Return "0" if success removing the data return -1 otherwise
   compare_data: Return "0" if the data are equal return -1 otherwise */
int sat_rem(Sat *sat, const uint8_t *data_to_rem,
            int (*compare_data)(const uint8_t *data1, const uint8_t *data2))
{
    Cell cell, prev;
    uint8_t data[sat->data_size], prev_data[sat->data_size], buff[sizeof(Cell)];
    int find;
    size_t pos;
    
    if (sat == NULL)
        return -1;

    memset(&prev, 'd', sizeof(Cell));
    memset(&buff, 'd', sizeof(Cell));

    /* First get the tail */
    if (__sat_get_tail(sat, &cell, data) == -1) {
        close(FD);
        return -1;
    }

    find = 0;
    do {
        /* Compare the data it its */
        if (compare_data(data_to_rem, data) == 0) {
            find = 1;
            break;
        }

        memcpy(&prev, &cell, sizeof(Cell));
        memcpy(prev_data, data, sat->data_size);
        
    } while (__sat_next_cell(sat, &cell, data) != -1);
    
    if (!find)
        return -1;
    
    /* Catch the next register */
    if (memcmp(&prev, buff, sizeof(Cell)) != 0) {
        if (__sat_set_next_cell(sat, prev.curernt_pos, cell.next_pos) == -1) {
            close(FD);
            return -1;
        }

        if (sat->head == (int) cell.curernt_pos)
            sat->head = prev.curernt_pos;
    } else {
        if (sat->size == 1)
            sat->head = -1;
        sat->tail = cell.next_pos;
    }

    /* Mark the cell */
    pos = cell.curernt_pos;
    memset(&cell, 'd', sizeof(Cell));
    if (__sat_overwrite_cell(sat, pos, &cell) == -1) {
        close(FD);
        return -1;
    }

    sat->size--;

    return 0;
}

/* sat_is_in: Return 1 if the data exist in the list else return 0 return -1 otherwise
   compare_data: Return "0" if the data are equal return -1 otherwise */
int sat_is_in(Sat *sat, const uint8_t *data,
              int (*compare_data)(const uint8_t *data1, const uint8_t *data2))
{
    Cell cell;
    uint8_t buff[sat->data_size];
        
    if (sat == NULL || data == NULL || sat->size == 0)
        return -1;

    if (__sat_get_tail(sat, &cell, buff) == -1) {
        close(FD);
        return -1;
    }

    do {
        if (compare_data(data, buff) == 0)
            return 1;
    } while (__sat_next_cell(sat, &cell, buff) != -1);

    return 0;
}

/* sat_app: Return "0" if success appending a new cell to the last part of the file return -1 otherwise */
int sat_app(Sat *sat, const uint8_t *data)
{
    Cell new_cell;
    
    if (sat == NULL || data == NULL)
        return -1;

    /* Catch the new cell */
    if (sat->head != -1) {
        /* Link to the new cell */
        if (__sat_set_next_cell(sat, sat->head, NUM_CELLS) == -1) {
            close(FD);
            return -1;
        }
    } 

    if (sat->tail == -1)
        sat->tail = NUM_CELLS;

    new_cell.curernt_pos = NUM_CELLS;
    new_cell.next_pos = -1;
    if (__sat_app_new_cell(sat, &new_cell, data) == -1)
        return -1;

    /* Add the new header */
    sat->head = NUM_CELLS++;
    sat->size++;

    /* Write the number of cells */
    if (__sat_write_headers() == -1)
        return -1;

    return 0;
}


/* sat_cosnt: Return "0" if success constructing a new instance return -1 otherwise */
int sat_const(Sat *sat, size_t data_size)
{
    if (sat == NULL)
        return -1;

    sat->tail = sat->head = -1;
    sat->size = 0;
    sat->data_size = data_size;

    /* Alloc space for the buffer of data */
    if (DATA == NULL && (DATA = malloc(data_size)) == NULL)
        return -1;
    
    return 0;
}

/* sat_dest: Return "0" if success destructing the instance return -1 otherwise */
int sat_dest(Sat *sat)
{
    /* Reset the structure */
    memset(sat, 0, sizeof(Sat));
    
    return 0;
}

/* open_saturation_file: Return "0" if success creating or openning a new staturation file
   return -1 otherwise */
int open_saturation_file(const char *file_name)
{
    /* Copy the memory */
    strcpy(FILE_NAME, file_name);
    NUM_CELLS = 0;
    FD = -1;
    DATA = NULL;

    /* Check if the file exist */
    if (access(file_name, R_OK) == 0) {
        if (FD == -1 && (FD = open(file_name, O_RDWR)) == -1)
            return -1;

        if (__sat_read_headers() == -1) {
            close(FD);
            return -1;
        }
    } else {
        /* Create the file */
        if (creat(file_name, 0644) == -1)
            return -1;

        if ((FD = open(file_name, O_WRONLY | O_APPEND)) == -1)
            return -1;
        
        if (__sat_write_headers() == -1) {
            close(FD);
            return -1;
        }

        if (close(FD) == -1)
            return -1;
        
        if ((FD = open(file_name, O_RDWR)) == -1)
            return -1;
    }
    
    return 0;
}

/* close_saturation_file: Return "0" if success closing the saturation file return -1 otherwise */
int close_saturation_file()
{
    memset(FILE_NAME, 0, FILE_NAME_LEN);
    NUM_CELLS = 0;

    if (__sat_write_headers() == -1)
        return -1;

    if (close(FD) == -1)
        return -1;

    FD = -1;
    free(DATA);
    
    return 0;
}
