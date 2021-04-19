#ifndef B4MESH_CONFIGS
#define B4MESH_CONFIGS

<<<<<<< Updated upstream
// default port
=======
// Default port
>>>>>>> Stashed changes
#define DEFAULT_PORT 5000

// Dbus
#define POLLING_DBUS 5

<<<<<<< Updated upstream
// merge
#define MERGE_TIMING 5

// payload size
#define PAYLOAD_MIN 300
#define PAYLOAD_MAX 600

// transaction generation
#define WAIT_FOR_FIRST_TRANSACTION 5 // wait a bit that everyone is started
#define LAMBDA_DIST 0.5 // = 1/mean

// b4mesh recurrent task (Ask4missingblock)
#define RECCURENT_TIMER 10

// blockgraph
#define SIZE_BLOCK 10        // 130
#define SIZE_MEMPOOL 500

// Avoiding collision
#define WAIT_BEFORE 200 //us
#define WAIT_AFTER 500 //us 

//only in b4mesh class
#define DEBUG std::cout << getSeconds() << " : "

// the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
=======
// Merge
#define MERGE_TIMING 5

// Payload size
#define PAYLOAD_MIN 300
#define PAYLOAD_MAX 600

// Transaction generation
#define WAIT_FOR_FIRST_TRANSACTION 5 // wait a bit that everyone is started
#define LAMBDA_DIST 0.9 // = 1/mean

// B4mesh recurrent task (Ask4missingblock)
#define RECCURENT_TIMER 10

// Blockgraph
#define SIZE_BLOCK 120        // 120
#define SIZE_MEMPOOL 1000

// Retransmit transaction timing
#define T_RETRANS 60

// Limit in seconds for the block creation
#define SAFETIME 5

// Trace option
#define LIVEBLOCK_FILE "/tmp/blockgraph"
#define LIVEMEMPOOL_FILE "./live_mempool.txt"

// Only in b4mesh class
#define DEBUG std::cout << getSeconds() << " : "

// The following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
>>>>>>> Stashed changes
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


#endif /*B4MESH_CONFIGS*/