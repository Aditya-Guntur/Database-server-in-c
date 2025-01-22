/*This the place where the whole program implementation will occur*/
#include "tree.h"

Tree root = {.n = {
    .tag = (TagRoot | TagNode),
    .north = (Node *)&root,
    .west = 0,
    .east = 0,
    .path = "/"
}
};

void print_tree(int fd, Tree *_root){
    int8 indentation;
    int8 buf[256];
    int16 size;
    Node *n;
    Leaf *l;

    indentation = 0;
    for(n = (Node *)_root; n; n = n->west){
        Print(indent(indentation++));
        Print(n->path);
        Print("\n");
        
        if(n->east ){
            for(l = n->east; l;l = l->east){
                Print(indent(indentation));
                Print(n->path);
                Print("/");
                Print(l->key);
                Print(" -> '");
                write(fd,(char *) l->value,(int)l->size);
                Print("'\n");
            }
        }

    }

}

int8 *indent(int8 n){
    int16 i;
    static int8 buf[256];
    int8 *p;

    if(n<1)
        return (int8*)"";

    assert(n<120);
    zero(buf,256);

    for(i=0, p = buf; i<n; i++,p+=2){
        strncpy((char *)p , "  ",2);    
    
    }
    return buf;
}

void zero(int8 *str , int16 size){
    int8 *p;
    int16 n;

    for(n =0,p = str; n<size; p ++, n++){
        *p =0;
    }
    return;
}

Node *create_node(Node *parent, int8 *path){
    Node *n;
    int16 size;


    errno = NoError;
    assert(parent);
    size = sizeof(struct s_node);
    n = (Node*)malloc((int)size);
    zero((int8 *)n, size);

    parent->west = n;
    n->tag = TagNode;
    n->north = parent;
    strncpy((char *)n->path , (char *)path, 255);
    return n;
}

Leaf *lookup_linear(int8 *path, int8 *key){
    
}

Leaf *find_last_linear(Node *parent){
    Leaf *l;   
    errno = NoError;
    assert(parent);
    if(!parent->east){
        reterr(NoError);
    }
    for(l = parent->east; l->east; l = l->east);
    assert(l);

    return l;
}

Leaf *create_leaf(Node *parent, int8 *key, int8 *value, int16 count){
    
    Leaf *l,  *new;
    int16 size;
    assert(parent);
    l = find_last_linear(parent);

    size = sizeof(struct s_leaf);
    new = (Leaf *)malloc(size);
    assert(new);

    if(!l){
        //directly connected
        parent->east = new;
    }
    else{
        //l is a leaf
        l->east = new;
    }

    zero((int8 *)new , size);
    new->tag = TagLeaf;
    new->west = (!l) ?
        (Tree *) parent:
    (Tree *)l;


    strncpy ((char *) new->key, (char *)key , 127 );

    new->value = (int8 *)malloc(count);
    zero(new->value , count);
    assert(new->value);
    strncpy((char *) new->value, (char *)value ,count);
    new->size = count;

    return new;

    
}


int main() {
    Node *n,*n2;
    Leaf *l1,*l2;
    int8 *key, *value;


    n = create_node((Node*)&root, (int8 *)"/Users");
    assert(n);
    n2 = create_node(n, (int8 *)"/Users/login");
    assert(n2);

    key = (int8 *)"Jonas";
    value = (int8 *)"abc77301aa";
    int16 size = (int16)strlen((char *) value);
    l1 = create_leaf(n2, key, value, size);
    assert(l1);

    key = (int8 *)"john";
    value = (int8 *)"aa034945c";
    size = (int16)strlen((char *)value);
    l2 = create_leaf(n2,key,value,size);
    assert(l2);


    print_tree(1,&root);

    free(n2);
    free(n);

    //printf("%s\n", l1->value);
    
}
