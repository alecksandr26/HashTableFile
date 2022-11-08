#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "../include/dat.h"

#define DAT_FILE_NAME "dat.bin"


/* TESTCASES */

void TESTCASE_removing()
{
    Dat dat;
    const int num1 = 1;
    const int num2 = 2;
    const int num3 = 3;

    dat_const(&dat, DAT_FILE_NAME, sizeof(int));

    dat_append(&dat, (const uint8_t *) &num1);
    dat_append(&dat, (const uint8_t *) &num2);
    dat_append(&dat, (const uint8_t *) &num3);

    /* Delete 2 */
    assert(dat_rem(&dat, 1) == 0);
    
    dat_dest(&dat);
}

void TESTCASE_appeding()
{
    Dat dat;
    const int num1 = 1;
    const int num2 = 2;
    const int num3 = 3;

    dat_const(&dat, DAT_FILE_NAME, sizeof(int));

    assert(dat_append(&dat, (const uint8_t *) &num1) == 0);
    assert(dat_append(&dat, (const uint8_t *) &num2) == 0);
    assert(dat_append(&dat, (const uint8_t *) &num3) == 0);
    
    dat_dest(&dat);
}


void TESTCASE_const_dest()
{
    Dat dat;

    /* Test constructing the fukcing data */
    assert(dat_const(&dat, DAT_FILE_NAME, sizeof(int)) == 0);
    assert(dat.fd != -1);
    assert(access(dat.file_name, R_OK) == 0);
    assert(dat_dest(&dat) == 0);
}

int main()
{
    
    puts("Testing: dat.o");
    
    TESTCASE_const_dest();
    remove(DAT_FILE_NAME);
    TESTCASE_appeding();
    remove(DAT_FILE_NAME);
    TESTCASE_removing();
    remove(DAT_FILE_NAME);
    
    return 0;
}


