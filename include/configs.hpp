#ifndef B4MESH_CONFIGS
#define B4MESH_CONFIGS

// default port
#define DEFAULT_PORT 5000

// Dbus
#define POLLING_DBUS 5

//payload size
#define PAYLOAD_MIN 300
#define PAYLOAD_MAX 600

//transaction generation
#define WAIT_FOR_FIRST_TRANSACTION 5 // wait a bit that everyone is started
#define LAMBDA_DIST 0.5 // = 1/mean

//b4mesh recurrent task (Ask4missingblock)
#define RECCURENT_TIMER 10

//blockgraph
#define SIZE_BLOCK 5        // 130
#define SIZE_MEMPOOL 500

//Avoiding collision
#define WAIT_BEFORE 200 //us
#define WAIT_AFTER 500 //us 

#endif /*B4MESH_CONFIGS*/