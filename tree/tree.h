/*tree.h*/
//This is to keep all the definitons etc
#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<assert.h>
#include<errno.h>

#define TagRoot 1
#define TagNode 2
#define TagLeaf 4

#define NoError 0//Here we are keeping only multiples of 2 as then we will only have a single 1 in every binary representatino
#define find_last(x)     find_last_linear(x)
#define reterr(x) \
    errno = (x);  \
    return NULL

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;
typedef unsigned char Tag;

struct s_node{
    Tag tag;
    struct s_node *north;
    struct s_node *west;
    struct s_leaf *east;
    int8 path[256];
};
typedef struct s_node Node;

struct s_leaf{
    Tag tag;
    union u_tree *west;
    struct s_leaf *east;
    int8 key[128];
    int8 *value;//This is a pointer because the values can be huge
    int16 size;//This is the size of the value
};
typedef struct s_leaf Leaf;

union u_tree{
    Node n;
    Leaf l;
};
typedef union u_tree Tree;