#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* Include the module */
#include "../include/map.h"

#ifndef WIN32
#define CLEAR "clear"
#else
#define CLEAR "cls"
#endif

#define LEN_NAME 30
#define LEN_PASSWORD 30
#define LEN_ID 30
#define MAX_NUM_CELLS 0

/* The files names */
#define MAP_FILE_NAME "map.bin"
#define DAT_FILE_NAME "dat.bin"
#define SAT_FILE_NAME "sat.bin"

typedef enum {
    NONE, /* 0 */
    ADD,
    DEL,
    UP,
    SEARCH,
    EXIT /* 5 */
} OPT;

/* User struct */
typedef struct {
    char name[LEN_NAME];
    char password[LEN_PASSWORD];
    char id[LEN_ID];
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

/* TODOO: Change the input methods for more secure  in c */

/* generate_id: Generates a new id for the user */
void generate_id(User *user)
{
    size_t i;

    for (i = 0; i < strlen(user->name) / 2; i++)
        user->id[i] = user->name[i];
    
    for ( ; i < strlen(user->password); i++)
        user->id[i] = user->password[i];
    
    /* Select one random caracter */
    srand(time(NULL));
    i = rand() % strlen(user->id);
    
    user->id[i] = toupper(user->id[i]);
}

/* capture_new_user: Just simple function to capture a new user */
void capture_new_user(User *user)
{
    system(CLEAR);
    
    memset(user, 0, sizeof(User));
    
    printf("Pon el nombre de usuario: ");
    scanf("%[^\n]%*c", user->name);
    
    printf("Pon el password: ");
    scanf("%[^\n]%*c", user->password);
}

/* print_user: Print the user */
void print_user(User *user)
{
    puts("--------------------------");
    printf("Nombre: %s\n", user->name);
    printf("Password: %s\n", user->password);
    printf("Id: %s\n", user->id);
}


/* tapo: Funcion para detener el programa */
void tapo()
{
    puts("\nPresiona cualquier tecla....");
    getchar();
}

int main()
{
    const char map_file_name[FILE_NAME_LEN] = MAP_FILE_NAME;
    const char dat_file_name[FILE_NAME_LEN] = DAT_FILE_NAME;
    const char sat_file_name[FILE_NAME_LEN] = SAT_FILE_NAME;
    char id[LEN_ID];
    
    OPT opc;
    Map *map;
    User user, *user_ptr;

    assert((map = alloc_map()) != NULL && "Error allocating a map instance");

    assert(map_const(map, sizeof(User), LEN_ID,
                     map_file_name,
                     dat_file_name,
                     sat_file_name,
                     MAX_NUM_CELLS,
                     (int (*)(const void *)) &hash) == 0
           && "Error initializing the map structure");

    /* Run the menu */
    do {
        system(CLEAR);
        puts("---------------");
        puts("[1] Agregar");
        puts("[2] Eliminar");
        puts("[3] Modificar");
        puts("[4] Buscar");
        puts("\n[5] Salir");
        puts("---------------");
        printf(">>> ");
        scanf("%i", (int *) &opc);
        getchar(); /* Catch the new line of enter */

        /* Switch */
        switch ((int) opc) {
        case ADD:
            capture_new_user(&user);
            generate_id(&user);
            assert(map_ins(map, &user, user.id) == 0 && "Error adding a new user");
            break;
        case DEL:
            break;
        case UP:
            break;
        case SEARCH:
            printf("Pon el id: ");
            scanf("%[^\n]%*c", id);
            assert((user_ptr = (User *) map_get_data(map, id)) != NULL && "Error Key doens't find it");
            print_user(user_ptr);
            tapo();
            break;
        case EXIT:
            system(CLEAR);
            break;
        default:
            puts("Error: opcion invalida!!!");
            tapo();
            break;
        }

        /* Clean the IO buffers */
        fflush(stdin);
        fflush(stdout);
    } while (opc != EXIT);

    assert(map_dest(map) == 0 && "Error destructing the map structure");

    assert(dealloc_map(map) == 0 && "Error freeing the memory of the alloc");
    
    return 0;
}
