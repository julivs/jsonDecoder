# jsonDecoder

jsonDecoder is a simple C program to parse JSON strings.

No need for instalation, only add header to your code.

To use, create an initial Node and assign the original string to node.content

run makeJSON(Node *node)

nodeLength(Node *node) returns collection size

getArrItem(Node *node, int index) return the respective array item as node

getObjProp(Node *node, char *key) returns the object's key item as node

node.content will have the final value of the node as a string

freeNode(Node *node) to deallocate.

The parseJSON.c is a use example.
