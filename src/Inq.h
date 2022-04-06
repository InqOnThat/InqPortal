#ifndef INQ_H
#define INQ_H

#include <Arduino.h>
#include <user_interface.h>
#include <Hash.h>

// rc = Return Code
// Returns negative values for error conditions as defined below.
// Returns GOOD = 0 for successful value.
// Returns positive values are specialized and method defined.
typedef s16 rc;        
const rc GOOD = 0;     

#define EQUAL !strcmp
#define ASSERT(c, f, ...) asrt(c, __FILE__, __LINE__, f, ## __VA_ARGS__)

void asrt(bool cond, const char* file, s32 line, const char* format, ...);

//{ =========================== ERROR CONSTANTS ================================

//{ Espressif Errors -----------------------------------------------------------

// Errors returned by InqOnThat.com libraries, but came from Espressif Core
// libraries.  Duplicated here (10/20/21) from espconn.h for convenience only.
/*                          DO NOT UNCOMMENT
#define ESPCONN_MEM               -1   // Out of memory error.     
#define ESPCONN_TIMEOUT           -3   // Timeout.                 
#define ESPCONN_RTE               -4   // Routing problem.         
#define ESPCONN_INPROGRESS        -5   // Operation in progress    
#define ESPCONN_MAXNUM		      -7   // Total number exceeds the set maximum

#define ESPCONN_ABRT              -8   // Connection aborted.      
#define ESPCONN_RST               -9   // Connection reset.        
#define ESPCONN_CLSD             -10   // Connection closed.       
#define ESPCONN_CONN             -11   // Not connected.           

#define ESPCONN_ARG              -12   // Illegal argument.        
#define ESPCONN_IF		         -14   // UDP send error			
#define ESPCONN_ISCONN           -15   // Already connected.       
#define ESPCONN_HANDSHAKE        -28   // ssl handshake failed	 
#define ESPCONN_SSL_INVALID_DATA -61   // ssl application invalid

// lwIP Errors -----------------------------------------------------------------

#define ERR_OK          0    // No error, everything OK. 
#define ERR_MEM        -1    // Out of memory error.     
#define ERR_BUF        -2    // Buffer error.            
#define ERR_TIMEOUT    -3    // Timeout.                 
#define ERR_RTE        -4    // Routing problem.         
#define ERR_INPROGRESS -5    // Operation in progress    
#define ERR_VAL        -6    // Illegal value.           
#define ERR_WOULDBLOCK -7    // Operation would block.   

#define ERR_IS_FATAL(e) ((e) < ERR_WOULDBLOCK)

#define ERR_ABRT       -8    // Connection aborted.      
#define ERR_RST        -9    // Connection reset.        
#define ERR_CLSD       -10   // Connection closed.       
#define ERR_CONN       -11   // Not connected.           

#define ERR_ARG        -12   // Illegal argument.        

#define ERR_USE        -13   // Address in use.          

#define ERR_IF         -14   // Low-level netif error    
#define ERR_ISCONN     -15   // Already connected.       

*/
//}

//   Espressif's errors are based on s8, so they will never go below -128.
//   We use an s16 for returning errors, thus we use between -129 and -32768.
//   Like Espressif, we return 0 indicating no error (see GOOD above).
//   Some functions return positive numbers which have meaning spcific to 
//   the function.

//{ InqAP ----------------------------------------------------------------------

#define ERR_PARSE_AP            -130
#define ESS_SET_OPMODE          -131

// SoftAP Connection
#define ERR_HOST_SSID           -132
#define ERR_HOST_PW             -133
#define ESS_HOST_STOP_DHCP      -134
#define ESS_HOST_SET_INFO       -135
#define ESS_HOST_LEASE          -136
#define ESS_HOST_LEASE_TIME     -137
#define ESS_HOST_LEASE_OFFR     -138
#define ESS_HOST_DHCP_START     -139
#define ESS_HOST_SLEEP          -140
#define ESS_HOST_SET_CFG        -141
#define ESS_HOST_SLEEP          -142

// StationAP Connection
#define ERR_CLI_SSID            -143
#define ERR_CLI_PW              -144
#define ESS_GET_CONFIG_CON      -145
#define ESS_GET_CONFIG_FRGT     -146
#define ESS_AP_DISCONNECT       -147
#define ESS_AP_ESP_AUTO         -148
#define ESS_AP_ESP_RECON        -149
#define ESS_AP_CURRENT          -150
#define ESS_AP_HOSTNAME         -151
#define ESS_AP_CONNECT          -152
#define ERR_AP_NOT_EXIST        -153
#define ERR_ALREADY_CONN        -154
#define ERR_CHANGE_ONE          -155

//}
//{ InqServer / InqClient ------------------------------------------------------

#define ERR_ALL_QUES_BUSY       -160
#define ERR_URL_FMT             -161    // Method not found
#define ERR_UNSUPPORTED         -162    // Method not supported
#define ERR_URL_TOO_LONG        -163    // Somewhere around 500 characters
#define ERR_404                 -164    // URL not found Canned or F.S.
#define ERR_UPLOAD_FMT          -165    // Data format invalid
#define ERR_UPG_INVALID         -166    // Upgrade invalid

//}
//{ InqPersist -----------------------------------------------------------------

#define ERR_PER_FLASH           -170    // Trying to allocate more than 4096
#define ERR_PER_MALLOC          -171    // Malloc failed
#define ERR_PER_SIZE            -172    // Temp size estimate bad.
#define ERR_PER_NOT_DIRTY       -173    // Dirty flag is not set.
#define ERR_PER_NO_DATA         -174    // Request returned NULL data

//}
//{ InqFS ----------------------------------------------------------------------

#define ERR_FILE_NE             -180    // Partial write of file
#define ERR_FILE_CRPT           -181    // Flash is corrupt
#define ERR_FILE_BOOK           -182    // Bad bookkeeping
#define ERR_FILE_MEM            -183    // Insufficient ram to do defrag
#define ERR_FILE_OPEN           -184    // File open fail - not enough flash

//}
//{ InqPortal ------------------------------------------------------------------

// Loading
#define ERR_CORRUPT             -202    // Data in flash not valid
#define ERR_PORT_USER           -203    // User data not consistent with saved

// Messages (Not Errors)
#define SVR_REBOOT              1001    // Reboot server (after upgrade)
#define RST_SOFTAP              1010    // SoftAP restart
#define DEFRAG_UNNECESSARY      1012    // Defrag is not necessary

//} 
//}
//{ ============================== LOG LEVELS ==================================

// List of log levels currently used by libraries.
// Lower numbers are most important to be seen.  Higher numbers are 
// informational.  Those closest to 255 are left to debugging inner-loops
#define LL_INQ_ERROR    201         // Rare, but want to see
#define LL_COMMS        209         // Setting up connectings - soft or AP.
#define LL_INQPORTAL    210         // InqPortal informational
#define LL_FS           211         // InqFS informational

#define LL_INQ          240         // classes in Inq.cpp
#define LL_INQCLOCK     242         // InqClock.cpp
#define LL_INQINTERVAL  243         // InqInterval.cpp

//} ============================== LOG LEVELS ==================================

#endif // INQ_H

