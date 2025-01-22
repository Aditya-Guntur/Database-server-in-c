#include "cache22.h"

bool scontinuation;
bool ccontinuation;
int32 handle_hello(Client *, int8 * , int8 *);

CmdHandler handlers[] = {
    {(int8 *)"hello",handle_hello}
};

Callback getcmd(int8 *cmd){
    Callback cb;
    int16 n,arrlen;

    if(sizeof(handlers)<16){
        return 0;
    }
    arrlen = (sizeof(handlers)/16);

    cb = 0;
    for(n=0; n<arrlen; n++){
        if(!strcmp((char *)cmd, (char *)handlers[n].cmd)){
            cb = handlers[n].handler;
            break;

        }
    }

    return cb;
}

int32 handle_hello(Client *cli, int8 *folder, int8 *args){
    dprintf(cli->s, "hello, '%s'\n",folder );
    return 0;
}

void zero(int8* buf, int16 size){
    int8* p;
    int16 n;

    for(n=0, p=buf;n<size; n++,p++){
        *p=0;

    }

    return;
}

void childloop(Client *cli){
    int8 buf[256];//creation of a buffer of 256 bytes
    int8 *p , *f;//p is the pointer in the buffer, f is for the folder.
    int16 n;//This is count of buffer size
    int8 cmd[256], folder[256], args[256];


    zero(buf,256);
    //To read from a file descriptor, we use the read function call.
    read(cli->s, (char *)buf, 255);
    n = (int16)strlen((char *)buf);
    if(n>254){
        n = 254;
    }

    for(p = buf;
        (*p)//This returns 1 if p points to something and 0 if p points to null.
            && (n--)//If the number is +ve, the n-- will be true for the && operator.
            && (*p != ' ')
            && (*p != '\n')
            && (*p != '\r');
        p++     
        );//This doesn't even need a body. It just brings the p pointer to a certain place.
    /*Once the above loop is done, there are cases:
    1. If p =0, it means that the command has filled out the whole buffer.*/
    zero(cmd,256);zero(folder,256);zero(args,256);

    if(!(*p)|| (!n)){
        strncpy((char *)cmd, (char *)buf , 255);
        goto done;
    }   

    else if((*p == '\n') || (*p =='\r')){
        *p = 0;
        strncpy((char*)cmd, (char*)buf, 255);
        goto done;
    }
    else if((*p = ' ')){
        *p = 0;
        strncpy((char*)cmd, (char*)buf, 255);
    }
    
    for(p++, f = p;
        (*p)
            && (n--)
            && (*p != ' ')
            && (*p != '\n')
            && (*p != '\r');
        p++     
        );

    if(!(*p)|| (!n)){
        strncpy((char *)folder, (char *)f , 255);
        goto done;
    }
    else if((*p == ' ')|| (*p == '\n') || (*p == '\r')){
        
        *p =0;
        strncpy((char *)folder, (char *)f , 255);
    }

    p++;
    if(*p){
        strncpy((char*)args , (char*)p , 255);
        for(p = args; (*p) && (*p !='\n') && (*p != '\r'); p++);
        *p = 0;

    }

    done:
        dprintf(cli->s, "\ncmd:\t%s\n", cmd);
        dprintf(cli->s, "folder:\t%s\n", folder);
        dprintf(cli->s, "args:\t%s\n", args);
    return; 
}

void mainloop(int s){
    struct sockaddr_in cli;
    int s2;
    /*Note: Even though s2 is a socket, we are representing it using an int. THis is because
    sockets are treated as file descriptors that are represented as integers.*/
    int32 len;
    char *ip;
    int16 port;
    Client *client;
    pid_t pid;

    s2 = accept(s, (struct sockaddr *)&cli , (unsigned int *)&len);
    /*This extracts the first connection request on the queue of pending connections
    for the listening socket, sockfd, creates a new socket and returns a new file 
    de-scriptor referring to that socket.*/
    if(s2<0){
        return;
    }

    port = (int16)htons((int)cli.sin_port);
    ip = inet_ntoa(cli.sin_addr);

    printf("connection from %s:%d\n", ip , port);

    client = (Client*)malloc(sizeof(Client));
    assert(client);

    zero((int8*)client, sizeof(Client));
    client->s = s2;
    client->port = port;
    strncpy(client->ip, ip, 15);

    pid = fork();
    /*The main use of the above function is that we have the above code for mainloop. Now,
    We want it to work for multiple clients. Thus, we make multiple copies of it. This can
    be done using fork. This is at the OS level, whereas different class instances happen
    during run time. Forked processes are completely independent of each other, whereas 
    instances of a class share the same memory and space and can easily interact.But this
    is heavy on the system. In multi core, these can run parallely rather than on a single
    thread like class instances.*/
    
    if(pid){
        //If pid returns 1 it means we are in the parent loop.
        free(client);

        return;
    }
    else{
        //If pid returns 0, it means we are in the child loop.
        dprintf(s2,"100 Connected to Cache22 server\n");//check in man3
        ccontinuation = true;
        while(ccontinuation)
            childloop(client);
        
        close(s2);
        free(client);

        return;
    }

    return;


}

int initserver(int16 port){
    struct sockaddr_in sock;
    int s;//THis is to hold our file scripter
    


    sock.sin_family = AF_INET;
    /*the above line tells that the socket is used for IPv4 addresses. Also, it determines 
    the domain or the scope of the socket. */

    sock.sin_port = htons(port);
    /* The sin_port field stores the port number in network byte order (big-endian). 
   If the host uses little-endian (common on x86), htons ("Host to Network Short") 
   converts the value to big-endian.

   Example: 0xFF05 in host byte order (stored as 05 FF) becomes 0x05FF in network byte order. */



    sock.sin_addr.s_addr = inet_addr(HOST);
    /* here we use inet_addr. This also has to get converted to network byte order. In hex
    the order will be 0x 01 02 03 04 and this has to get converted to 04 03 02 01*/



    s = socket(AF_INET , SOCK_STREAM , 0);
    /*THis creates an endpoint for communication. On success, the file descriptor for the 
    socket is returned. On error -1 is returned and errno is set to indicate that error.*/
    assert(s>0);

    //Now we must bind the socket to the structure using bind
    errno =0;
    if(bind(s, (struct sockaddr *)&sock , sizeof(sock)) !=0){
    //This is a little weird. It returns 0 on success. 
        assert_perror(errno);
    };/*Bind is used to associate the socket to the specific IP address and port.
    The reason we typecasted sock here is because we specially mentioned sock to be of
    IPV4 address type, whereas bind can take care of any address type. THus it was changed.
    This works because the fields in both are the same.*/
    


    errno =0;
    if(listen(s , 20)){//THis is used to check for connections on a socket.
        assert_perror(errno);
    };//This means we can accept 20 connections at a time. 

    printf("server listening on %s:%d\n", HOST , port);

    return s;
}

int main(int argc, char *argv[]){
    char *sport;
    int16 port;
    int s;

    Callback x;

    x = getcmd((int8*)"hello");
    printf("%p\n");
    return 0;

    
    if(argc < 2){
        sport = PORT;
    }
    else{
        sport = argv[1];//This means we can give our own port of choice
    }
    port = (int16)atoi(sport);

    s = initserver(port);

    scontinuation = true;
    while(scontinuation){
        mainloop(s);
    }
    printf("Shutting down...\n");
    close(s);

}  