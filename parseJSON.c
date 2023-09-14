#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "parseJSON.h"

int main(int argc, char **argv)
{
    Node node;

    char content[50] = "\0";
    strcpy(content, "{'name':'john','age':25}");

    node.content = content;
    makeJSON(&node);

    Node *data;
    
    data = getObjProp(&node, "name");
    printf("name = %s\n", data->content);

    data = getObjProp(&node, "age");
    printf("age = %s\n", data->content);

    freeNode(&node);

    return 0;
}