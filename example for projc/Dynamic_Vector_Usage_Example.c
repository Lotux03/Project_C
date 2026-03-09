#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    void* data;
    size_t size;
    size_t cap;
    size_t elem_size;
} Array;

Array arr;

void init(Array* arr, size_t elem_size, int initSize) {
    arr->elem_size = elem_size;
    arr->data = malloc(elem_size * initSize);
    arr->size = 0;
    arr->cap = initSize;
}

void push_back(Array* arr, void* content) {
    if (arr->size == arr->cap) {
        void* temp = realloc(arr->data, (arr->elem_size * arr->cap * 2));
        if (!temp){
            printf("realloc failed lol");
            exit(1);
        }
        arr->data = temp;
        arr->cap *= 2;
    }
    memcpy(
        (char*)arr->data + (arr->size * arr->elem_size),
        content,
        arr->elem_size
    );
    arr->size++;
}

void free_array(Array* arr) {
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->cap = 0;
    arr->elem_size = 0;
}

typedef struct {
    int id;
    char name[32];
} User;

int main(void) {
    srand((unsigned int)time(NULL));

    const char* name_pool[] = {
"Liam","Olivia","Noah","Emma","Oliver","Ava","Elijah","Sophia","William","Isabella",
"James","Mia","Benjamin","Charlotte","Lucas","Amelia","Henry","Harper","Alexander","Evelyn",
"Michael","Abigail","Daniel","Emily","Matthew","Ella","Jackson","Elizabeth","Sebastian","Camila",
"Aiden","Luna","David","Sofia","Joseph","Avery","Samuel","Mila","Carter","Aria",
"Owen","Scarlett","Wyatt","Penelope","John","Layla","Jack","Chloe","Luke","Victoria",
"Jayden","Madison","Dylan","Eleanor","Grayson","Grace","Levi","Nora","Isaac","Riley",
"Gabriel","Zoey","Julian","Hannah","Mateo","Hazel","Anthony","Lily","Jaxon","Ellie",
"Lincoln","Violet","Joshua","Lillian","Christopher","Zoe","Andrew","Stella","Theodore","Aurora",
"Caleb","Natalie","Ryan","Emilia","Asher","Everly","Nathan","Leah","Thomas","Aubrey",
"Leo","Willow","Isaiah","Addison","Charles","Lucy","Josiah","Audrey","Hudson","Bella"
};

    size_t name_count = sizeof(name_pool) / sizeof(name_pool[0]);

    Array users;
    init(&users, sizeof(User), 2);

   
    int user_count = rand() % 8 + 3;

    for (int i = 0; i < user_count; i++) {
        User u;
        u.id = rand() % 10000;

        const char* random_name = name_pool[rand() % name_count];
        strncpy_s(u.name, random_name, sizeof(u.name));
        u.name[sizeof(u.name) - 1] = '\0';

        push_back(&users, &u);
    }

    printf("Generated %d users:\n\n", user_count);
    for (size_t i = 0; i < users.size; i++) {
    User* u = &((User*)users.data)[i];
    printf("%s\n", u->name);
    printf("User index: %zu\n", i);

    printf("  sizeof(User): %zu bytes\n", sizeof(User));
    printf("  sizeof(id): %zu bytes\n", sizeof(u->id));

    printf("  Address of User: %p\n", (void*)u);
    printf("  Address of id:   %p\n", (void*)&u->id);

    printf("  id value: %d\n", u->id);

    printf("\n");
}

    free_array(&users);
    return 0;
}



