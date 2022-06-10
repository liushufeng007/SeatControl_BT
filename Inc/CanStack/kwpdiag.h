#if !defined(KWPDIAG_H)
#define KWPDIAG_H

#include "vnim_types.h"
#include "kwpdiag_cfg.h"

typedef unsigned char kwpdiagu8;
typedef unsigned short kwpdiagu16;


void kwpdiag_postprocessing(unsigned char bufferindex,unsigned char respdata); 
void kwpdiag_P2timeout(void);
void kwpdiag_P3timeout(void);
void kwpdiag_dispatcher(unsigned char bufferidx);

void kwpdiag_testerpresenthandler(unsigned char *pdiagdata,unsigned short diagrequestlength);
void kwpdiag_FuncNotSupported(unsigned char *pdiagdata,unsigned short diagrequestlength);

void kwpdiag_sendnegativeresponse(unsigned char sid,unsigned char errorcode); 
kwpdiagu8 kwpdiag_sendrcrrp (void);

void kwpdiag_init(void);
void kwpdiag_periodic(void);
void kwpdiag_setnegativeresponse(unsigned char bufferindex,unsigned char errorcode);
void kwpdiag_processingdone(unsigned char bufferindex,unsigned short reslen );
kwpdiagu8 kwpdiag_DiagSession(void);

extern kwpdiagu8 kwpdiag_currreq;
extern kwpdiagu8 kwpdiag_actul_req;
extern kwpdiagu8 kwpdiag_currsess;
extern kwpdiagu8 kwpdiag_programmingrequest;
extern kwpdiagu8 const kwpdiag_numsrv;


/* ===========================================================================
//  P U B L I C   T Y P E   D E F I N I T I O N S
// =========================================================================*/

#define UPDATE_SESSION(a)   (kwpdiag_currsess = a)
#define GET_DIAG_SESSION()  (kwpdiag_currsess)


#define KWPDIAG_ENABLE  1
#define KWPDIAG_DISABLE 0
#define KWPDIAG_TRUE  1
#define KWPDIAG_FALSE 0

#define KWPDIAG_INVALID 0xFF
#define KWPDIAG_NOERROR 0x00
#define KWPDIAG_NOREQ 0xFF

#define KWPDIAG_DEFAULTSESS_ID 0x01
#define KWPDIAG_PROGSESS_ID 0x02
#define KWPDIAG_EXTENDEDSESS_ID 0x03

#define KWPDIAG_PROGSESS_INDEX 0
#define KWPDIAG_DEFAULTSESS_INDEX 1

#define KWPDIAG_SRVNOTSUPPORTEDINACTIVEDIAGMODE 0x22

#define KWPDIAG_NEGRESPLEN 3
#define KWPDIAG_NEGRESPCODE  0x7F
#define KWPDIAG_RCRRPRESPONSE 0x78
#define KWPDIAG_NRCTRANSFERABORTED 0x72
#define KWPDIAG_NRCSRVNOTSUPPORTED 0x11
#define KWPDIAG_NRCSUBFUNCNOTSUPPORTED 0x12
#define KWPDIAG_NRCINVALIDFORMAT 0x13
#define KWPDIAG_NRCREQOUTOFRANGE 0x31
#define KWPDIAG_NRCNOTSUPPORTEDINACTIVESESS 0x7F


typedef void  (*kwpdiag_posthandler_t ) (void);
typedef void  (*kwpdiag_mainhandler_t ) (unsigned char*, unsigned short);

/**********************************************************************/
/*           Buffer handling data structure                           */
/**********************************************************************/
typedef struct kwpdiagbuffcntrl
{
    /*Service ID index of the current request*/
    unsigned char currentsrvidx;
    /*Flag to start the response framing*/
    unsigned char makeresponse         :1;
    /*Flag to indicate the confirmation of response txed*/
    unsigned char confirmation_rxed    :1;
    /*Flag to start transmission, by calling TP*/
    unsigned char tx_ready             :1;
    /*Flag for buffer handling*/
    unsigned char bufferlocked         :1;
    /*Flag to indicate new request received */
    unsigned char ind_flag             :1;
    
}KWPDIAG_bufferctrl_t;
/**********************************************************************/
/*           Function handler for service                             */
/**********************************************************************/
typedef struct kwpdiagfunchndlr 
{
    /*Pointer to the main function handler for the requested service*/
    kwpdiag_mainhandler_t   pkwpdiag_mainfunc;
    /*Pointer to the post handler to be called after confirmation of request transmission.*/
    kwpdiag_posthandler_t pkwpdiag_posthandle;
}KWPDIAG_funchandler_t;

/**********************************************************************/
/*           Service Information's data structure                     */
/**********************************************************************/
typedef struct kwpdiagsrvcinfo
{
  /*SID requests supported by the default session.*/
  unsigned char requestsid;
  /*SID to be sent with the positive response for a request*/
  unsigned char responsesid;
  /*This is to specify the sessions in which this SID is supported*/
  unsigned char phy_sesssupp;
  unsigned char fun_sesssupp;
  #if( (KWPDIAG_RESPONSE_ON_FUNC_REQ == KWPDIAG_ENABLE) && \
       (KWPDIAG_RESPONSE_ALL_ON_FUNC_REQ == KWPDIAG_DISABLE))
  /*Flag to be used in case of response enabled for functional request.*/
  unsigned char responsefuncreq;
  #endif
  /*Function handlers for the services.*/
  KWPDIAG_funchandler_t funchandler;
}KWPDIAG_serviceinfo_t;



typedef enum
{
  KWPDIAG_PRIMARYREQ=0,
  KWPDIAG_SECONDARYREQ,
  KWPDIAG_NUMREQTYPE
}kwpdiag_reqtype;


extern KWPDIAG_serviceinfo_t const kwpdiag_srvinfotable[];





#endif /* KWPDIAG_H */

/* End of file ============================================================ */

/*****************************************************************************
C L E A R C A S E    R E V I S I O N     N O T E S
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
For each change to this file, be sure to record:
1.   Who made the change and when the change was made.
2.   Why the change was made and the intended result.

Date       By         Reason For Change
----------------------------------------------------------------------------
25-Feb-09
Clearcase Rev 1.0   ssaranya        Initial version.
26-Feb-09                            
Clearcase Rev 1.1   ssaranya        Peer Reveiw Changes.
02-Mar-09
Clearcase Rev 2.0   vkannan        Updated after creation of seperate files 
                                   for application services configuration

26-Mar-09     3     vkannan      Updated with QAC fixes after using latest 
                                   personality files from integration team.

03-Apr-09     3     vkannan      SMIS defects 19792 

03-Apr-09     4     vkannan      Moved TP channel config to Application 
                                specific Diag config file 

17-Apr-09    5         vkannan     Peer Reveiw Changes of typedef changes in 
                                   SMIS review 19792.
******************************************************************************/
