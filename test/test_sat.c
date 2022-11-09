#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>


#include "../include/sat.h"

#define SAT_FILE_NAME "sat.bin"


int compare_int(const int *int1, const int *int2)
{
    return !(*int1 == *int2);
}


/* TESTCASES */

void TESTCASE_get_data()
{
    Sat sat;
    
    const int num1 = 1;
    const int num2 = 2;
    const int num3 = 3;

    open_saturation_file(SAT_FILE_NAME);
    sat_const(&sat, sizeof(int));
    
    sat_app(&sat, (const uint8_t *) &num1);
    sat_app(&sat, (const uint8_t *) &num2);
    sat_app(&sat, (const uint8_t *) &num3);

    /* Try to bring the buffer of data */
    assert(*((int *) sat_get_data(&sat, (const uint8_t *) &num1,
                                  (int (*)(const uint8_t *, const uint8_t *)) &compare_int)) == num1);
    
    assert(*((int *) sat_get_data(&sat, (const uint8_t *) &num2,
                                  (int (*)(const uint8_t *, const uint8_t *)) &compare_int)) == num2);
    
    assert(*((int *) sat_get_data(&sat, (const uint8_t *) &num3,
                                  (int (*)(const uint8_t *, const uint8_t *)) &compare_int)) == num3);

    
    sat_dest(&sat);
    close_saturation_file(SAT_FILE_NAME);
}

void TESTCASE_multiple_lists_in_file()
{
    Sat sat, sat2;
    const int num1 = 1;
    const int num2 = 2;
    const int num3 = 3;

    open_saturation_file(SAT_FILE_NAME);
    
    sat_const(&sat, sizeof(int));
    assert(sat_const(&sat2, sizeof(int)) == 0);

    assert(sat_app(&sat, (const uint8_t *) &num1) == 0);
    assert(sat_app(&sat2, (const uint8_t *) &num1) == 0);
    assert(sat_app(&sat, (const uint8_t *) &num2) == 0);
    assert(sat_app(&sat2, (const uint8_t *) &num2) == 0);
    assert(sat_app(&sat, (const uint8_t *) &num3) == 0);
    assert(sat_app(&sat2, (const uint8_t *) &num3) == 0);

    assert(sat_size(&sat) == 3);
    assert(sat_size(&sat2) == 3);

    assert(sat_is_in(&sat, (const uint8_t *) &num3,
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 1);
    
    assert(sat_is_in(&sat, (const uint8_t *) &num2,
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 1);

    assert(sat_is_in(&sat, (const uint8_t *) &num1,
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 1);
    
    assert(sat_is_in(&sat2, (const uint8_t *) &num3,
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 1);
    
    assert(sat_is_in(&sat2, (const uint8_t *) &num2,
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 1);

    assert(sat_is_in(&sat2, (const uint8_t *) &num1,
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 1);



    assert(sat_rem(&sat, (const uint8_t *) &num1,
                   (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 0);
    
    assert(sat_rem(&sat2, (const uint8_t *) &num2,
                   (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 0);

    assert(*((int *) sat_get_tail(&sat)) == num2);
    assert(*((int *) sat_get_tail(&sat2)) == num1);
    
    sat_dest(&sat);

    assert(sat_dest(&sat2) == 0);
    
    close_saturation_file(SAT_FILE_NAME);
}

void TESTCASE_rem_consutl()
{
    Sat sat;
    const int num1 = 1;
    const int num2 = 2;
    const int num3 = 3;

    open_saturation_file(SAT_FILE_NAME);

    sat_const(&sat, sizeof(int));

    sat_app(&sat, (const uint8_t *) &num1);
    sat_app(&sat, (const uint8_t *) &num2);
    sat_app(&sat, (const uint8_t *) &num3);

    
    assert(sat_rem(&sat, (const uint8_t *) &num1,
                   (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 0);

    assert(sat_is_in(&sat, (const uint8_t *) &num1, 
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 0);

    assert(sat_rem(&sat, (const uint8_t *) &num3,
                   (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 0);
    
    sat_dest(&sat);

    close_saturation_file(SAT_FILE_NAME);
}


void TESTCASE_app_consult()
{
    Sat sat;
    const int num1 = 1;
    const int num2 = 2;
    const int num3 = 3;

    open_saturation_file(SAT_FILE_NAME);
    
    sat_const(&sat, sizeof(int));

    /* Check the appends  */
    assert(sat_app(&sat, (const uint8_t *) &num1) == 0);
    assert(sat_app(&sat, (const uint8_t *) &num2) == 0);
    assert(sat_app(&sat, (const uint8_t *) &num3) == 0);
    assert(sat_size(&sat) == 3);

    assert(sat_is_in(&sat, (const uint8_t *) &num3,
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 1);
    
    assert(sat_is_in(&sat, (const uint8_t *) &num2,
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 1);

    assert(sat_is_in(&sat, (const uint8_t *) &num1,
                     (int (*)(const uint8_t *, const uint8_t *)) &compare_int) == 1);

    sat_dest(&sat);

    close_saturation_file(SAT_FILE_NAME);
}

void TESTCASE_const_dest()
{
    Sat sat;
    
    open_saturation_file(SAT_FILE_NAME);
    
    assert(sat_const(&sat, sizeof(int)) == 0);

    assert(sat_dest(&sat) == 0);

    assert(access(SAT_FILE_NAME, R_OK) == 0);

    close_saturation_file(SAT_FILE_NAME);
}

void TESTCASE_open_close_file()
{
    assert(open_saturation_file(SAT_FILE_NAME) == 0);
    assert(close_saturation_file(SAT_FILE_NAME) == 0);
    
}

int main()
{
    puts("Testing: sat.o");
    

    TESTCASE_open_close_file();
    remove(SAT_FILE_NAME);
    TESTCASE_const_dest();
    remove(SAT_FILE_NAME);
    TESTCASE_app_consult();
    remove(SAT_FILE_NAME);
    TESTCASE_rem_consutl();
    remove(SAT_FILE_NAME);
    TESTCASE_multiple_lists_in_file();
    remove(SAT_FILE_NAME);
    TESTCASE_get_data();
    remove(SAT_FILE_NAME);

    /* Reset the test case */
    
    return 0;
}
