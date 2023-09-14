#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// No need for instalation, only add de header parseJSON.h to your code
//
// To use, create an initial Node and assign the original string to node.content
// run makeJSON(Node *node)
// nodeLength(Node *node) returns collection size
// getArrItem(Node *node, int index) return the respective array item as node
// getObjProp(Node *node, char *key) returns the object's key item as node
// node.content will have the final value of the node as a string
// freeNode(Node *node) to deallocate.

#ifndef JSON_PARSE
#define JSON_PARSE

enum NodeType
{
    TypeArr,
    TypeObj,
    TypeNum,
    TypeStr,
    TypeUndef
};

typedef enum NodeType NodeType;

struct Node
{
    NodeType typeNode;
    char *key;
    int index;
    char *content;
    struct Node *next;
    struct Node *child;
};

typedef struct Node Node;

static char *removeDelimiters(char *source, char *tipo)
{
    int len = strlen(source);
    if (len < 2)
        return source;
    if (tipo == NULL)
    {
        source[len - 1] = '\0';
        source[0] = '\0';
        return &source[1];
    }
    else if (source[0] == tipo[0] && source[len - 1] == tipo[0])
    {
        source[len - 1] = '\0';
        source[0] = '\0';
        return &source[1];
    }
    else
        return source;
}

static char *removeSpaces(char *data)
{
    int size = strlen(data);
    char *result = (char *)calloc(size + 1, sizeof(char));
    int i;
    int count = 0;
    int scaping = 0;
    char mark = '\0';
    char pre = '\0';
    int inside = 0;
    for (i = 0; i < size; i++)
    {
        char carac = data[i];
        scaping = 0;
        if (pre == '\\')
            scaping = 1;
        if (carac == '"' && inside == 0 && scaping == 0)
        {
            mark = '"';
            inside = 1;
        }
        else if (carac == '"' && inside == 1 && scaping == 0 && mark == '"')
        {
            mark = '\0';
            inside = 0;
        }
        if (carac == '\'' && inside == 0 && scaping == 0)
        {
            mark = '\'';
            inside = 1;
        }
        else if (carac == '\'' && inside == 1 && scaping == 0 && mark == '\'')
        {
            mark = '\0';
            inside = 0;
        }
        if (inside == 0 && carac != ' ' && carac != '\t' && carac != '\n')
        {
            result[count] = carac;
            count++;
        }
        else if (inside == 1)
        {
            result[count] = carac;
            count++;
        }
        if (pre == '\\' && carac == '\\')
            pre = '\0';
        else
            pre = carac;
    }
    return result;
}

static char *getNext(char **start, char delimiter)
{
    int maxDeep = 1000, changed = 0, count = 0;
    int final = strlen(*start);
    char stack[maxDeep];
    memset(stack, (int)'\0', maxDeep * sizeof(char));
    int deep = 0;
    char preCarac = '\0';
    while (count < final)
    {
        char carac = (*start)[count];
        changed = 0;
        if (carac == '\\')
        {
            count += 2;
            continue;
        }
        else if (carac == '"' && preCarac != '"')
        {
            stack[deep] = carac;
            deep++;
            changed = 1;
        }
        else if (carac == '"' && preCarac == '"')
        {
            stack[deep] = '\0';
            deep--;
            changed = 1;
        }
        else if (carac == '{')
        {
            stack[deep] = carac;
            deep++;
            changed = 1;
        }
        else if (carac == '}' && preCarac == '{')
        {
            stack[deep] = '\0';
            deep--;
            changed = 1;
        }
        else if (carac == '[')
        {
            stack[deep] = carac;
            deep++;
            changed = 1;
        }
        else if (carac == ']' && preCarac == '[')
        {
            stack[deep] = '\0';
            deep--;
            changed = 1;
        }
        if (deep > 0 && changed == 1)
        {
            preCarac = stack[deep - 1];
        }
        if (deep == 0 && carac == delimiter)
        {
            (*start)[count] = '\0';
            *start = *start + count + 1;
            return *start + count + 1;
        }
        count++;
    }
    *start = *start + count;
    return *start + count;
}

static NodeType getType(char carac)
{
    if (carac == '[')
        return TypeArr;
    else if (carac == '{')
        return TypeObj;
    else if (carac == '"' || carac == '\'')
        return TypeStr;
    else if ((carac >= '0' && carac <= '9') || carac == '-')
        return TypeNum;
    else
        return TypeUndef;
}

static void getContent(Node *node)
{
    node->typeNode = getType(node->content[0]);
    node->child = NULL;
    if (node->typeNode == TypeUndef)
        return;
    if (node->typeNode == TypeArr)
    {
        char *part = removeDelimiters(node->content, NULL);
        Node *previous = NULL;
        int count = 0;
        while (part[0] != '\0')
        {
            Node *newNode = (Node *)calloc(1, sizeof(Node));
            newNode->content = part;
            newNode->index = count;
            newNode->key = NULL;
            newNode->next = NULL;
            if (previous != NULL)
            {
                previous->next = newNode;
            }
            else
            {
                node->child = newNode;
            }
            previous = newNode;
            count++;
            getNext(&part, ',');
        }
        Node *current = node->child;
        while (current != NULL)
        {
            getContent(current);
            current = current->next;
        }
    }
    else if (node->typeNode == TypeObj)
    {
        char *part = removeDelimiters(node->content, NULL);
        Node *previous = NULL;
        int count = 0;
        while (part[0] != '\0')
        {
            Node *newNode = (Node *)calloc(1, sizeof(Node));
            newNode->index = count;
            newNode->key = part;
            getNext(&part, ':');
            newNode->next = NULL;
            newNode->content = part;
            if (previous != NULL)
            {
                previous->next = newNode;
            }
            else
            {
                node->child = newNode;
            }
            previous = newNode;
            count++;
            getNext(&part, ',');
        }
        Node *current = node->child;
        while (current != NULL)
        {
            getContent(current);
            current = current->next;
        }
    }
}

static void freeNodeRecur(Node *node)
{
    if (node != NULL)
    {
        Node *next = node->next;
        Node *child = node->child;
        free(node);
        freeNodeRecur(next);
        freeNodeRecur(child);
    }
}

void freeNode(Node *node)
{
    if (node != NULL)
    {
        if (node->next != NULL)
            freeNodeRecur(node->next);
        if (node->child != NULL)
            freeNodeRecur(node->child);
    }
}

static void trimValues(Node *node)
{
    if (node == NULL)
    {
        return;
    }
    if (node->typeNode == TypeStr)
    {
        node->content = removeDelimiters(node->content, NULL);
    }
    if (node->key != NULL)
    {
        node->key = removeDelimiters(node->key, "'");
        node->key = removeDelimiters(node->key, "\"");
    }
    trimValues(node->child);
    trimValues(node->next);
}

void makeJSON(Node *node)
{
    node->key = NULL;
    node->next = NULL;
    node->content = removeSpaces(node->content);
    getContent(node);
    trimValues(node);
}

Node *getArrItem(Node *base, int index)
{
    if (base->typeNode == TypeArr)
    {
        Node *current = base->child;
        do
        {
            if (current->index == index)
                return current;
            current = current->next;
        } while (current != NULL);
    }
    return NULL;
}

int nodeLength(Node *node)
{
    if (node == NULL)
        return 0;
    if (node->typeNode == TypeArr || node->typeNode == TypeObj)
    {
        Node *current = node->child;
        int count = 0;
        while (current != NULL)
        {
            count++;
            current = current->next;
        }
        return count;
    }
    else
        return -1;
}

Node *getObjProp(Node *base, char *key)
{
    if (base->typeNode == TypeObj)
    {
        Node *current = base->child;
        do
        {
            if (current->key != NULL)
            {
                if (strcmp(key, current->key) == 0)
                {
                    return current;
                }
            }
            current = current->next;
        } while (current != NULL);
    }
    return NULL;
}

#endif