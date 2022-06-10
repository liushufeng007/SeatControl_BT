/* ===========================================================================
//
//  Name:           kwpdiag.c
//
//  Description:    KWP Diagnostics Layer
//
//  Organization:   Multiplex Core Technology
//
// =========================================================================*/

/*****************************************************************************/
/* Include Files                                                             */
/*****************************************************************************/
#include <string.h>   					/* necessary for memcpy() */
#include "kwpdiag.h"
#include "kwpdiag_par.h"
#include "tp_par.h"
#include "system.h"





/**************config**********************************/
#define KWPDIAG_P2_TIMECNT      (KWPDIAG_P2_TIME_INMSEC/KWPDIAG_CALLCYCLE_INMSEC)
#define KWPDIAG_S1_TIMECNT      (KWPDIAG_S1_TIME_INMSEC/KWPDIAG_CALLCYCLE_INMSEC)
#define KWPDIAG_RCRRPMAXTIMECNT ((KWPDIAG_RCRRPMAXTIME_INMSEC/KWPDIAG_CALLCYCLE_INMSEC) + KWPDIAG_P2_TIMECNT)
                                  
#define KWPDIAG_P3MAXTIMECNT    (KWPDIAG_P3MAXTIME_INMSEC/KWPDIAG_CALLCYCLE_INMSEC)
/***********end of config******************************/
kwpdiagu8 kwpdiag_currreq;
kwpdiagu8 kwpdiag_actul_req;



/*Lets access this via */
kwpdiagu8 kwpdiag_currsess;
static kwpdiagu16 kwpdiag_p2timer;
static kwpdiagu16 kwpdiag_p3timer;
static kwpdiagu8 kwpdiag_skipsearch;
static KWPDIAG_bufferctrl_t kwpdiag_buffcntrl[KWPDIAG_NUMREQTYPE];
static kwpdiagu8  kwpdiag_bufferror[KWPDIAG_NUMREQTYPE];
static kwpdiagu16 kwpdiag_bufflen[KWPDIAG_NUMREQTYPE];
static kwpdiagu8 kwpdiag_primarybuffer[KWPDIAG_MAXPRIMARYBUFFLEN];
static kwpdiagu8 kwpdiag_secondarybuffer[KWPDIAG_MAXSECONDARYBUFFLEN];
static kwpdiagu8 kwpdiag_negresp[3];
kwpdiagu8 kwpdiag_programmingrequest; 
//static kwpdiagu16 kwpdiag_rplimiter;
static kwpdiagu8 kwpdiag_rcrpbuffer[3];
static kwpdiagu8 kwpdiag_rcrrpruning;


/*****************************************************************************/
/* Private Macros                                                            */
/*****************************************************************************/
#define posthandler   (kwpdiag_srvinfotable[kwpdiag_buffcntrl[index].currentsrvidx].funchandler.pkwpdiag_posthandle)
#define mainhandler   (kwpdiag_srvinfotable[srv_index].funchandler.pkwpdiag_mainfunc)


#define STOPP2TIMER() (kwpdiag_p2timer=0)
#define STOPP3TIMER() (kwpdiag_p3timer=0)
#define STARTP2TIMERONLY() (kwpdiag_p2timer=KWPDIAG_P2_TIMECNT)

#define STARTP3TIMER() (kwpdiag_p3timer=KWPDIAG_S1_TIMECNT)

#define GETP2TIMER() (kwpdiag_p2timer)
#define GETP3TIMER() (kwpdiag_p3timer)
#define DECP2TIMER() (kwpdiag_p2timer--)
#define DECP3TIMER() (kwpdiag_p3timer--)
#define INCP2TIMER() (kwpdiag_p2timer++)


#if (KWPDIAG_RCRRP == KWPDIAG_ENABLE)
#define STARTRPLIMITER() (kwpdiag_rplimiter=KWPDIAG_RCRRPMAXTIMECNT)
#define DECRPLIMITER() (kwpdiag_rplimiter--)
#define STARTP2TIMERRCRRPMAX() (kwpdiag_p2timer=KWPDIAG_P3MAXTIMECNT)
#define GETRPLIMITER() (kwpdiag_rplimiter)
#else
#define STARTRPLIMITER() 
#define DECRPLIMITER() 
#define STARTP2TIMER() STARTP2TIMERONLY() 
#define STARTP2TIMERRCRRPMAX()
#endif                        


/*****************************************************************************/


/*****************************************************************************/
/* Private Variable Definitions                                              */
/*****************************************************************************/
static kwpdiagu8  *pkwpdiagbuffer[KWPDIAG_NUMREQTYPE]=
{
    &kwpdiag_primarybuffer[0],
    &kwpdiag_secondarybuffer[0]
};

kwpdiagu8 const kwpdiag_numsrv = KWPDIAG_NUM_SERVICES;

KWPDIAG_serviceinfo_t const kwpdiag_srvinfotable[KWPDIAG_NUM_SERVICES] =
{
    KWPDIAG_SRV_INFO_TABLE_LIST
};
/*****************************************************************************/
/* Functions                                                                 */
/*****************************************************************************/
#if (KWPDIAG_RCRRP == KWPDIAG_ENABLE)
static void STARTP2TIMER (void)
{
    STARTP2TIMERONLY();
    STARTRPLIMITER();
}								     
#endif
/* ===========================================================================
//
// Name:            kwpdiag_init
//
// Description:     Initialize the Diagnostics layer
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_init(void)
{
    kwpdiagu8 index;
    STOPP2TIMER();
    STOPP3TIMER();
    for(index=0;index<KWPDIAG_NUMREQTYPE;index++)
    {
        kwpdiag_buffcntrl[index].makeresponse=0;
        kwpdiag_buffcntrl[index].confirmation_rxed=0;
        kwpdiag_buffcntrl[index].tx_ready=0;
        kwpdiag_buffcntrl[index].bufferlocked=0;
        kwpdiag_buffcntrl[index].ind_flag=0;
        kwpdiag_buffcntrl[index].currentsrvidx=KWPDIAG_INVALID;           
        kwpdiag_bufferror[index]=KWPDIAG_NOERROR;
    }
    kwpdiag_currreq=KWPDIAG_SECONDARYREQ;
    kwpdiag_currsess=DEFAULTSESSION;
    kwpdiag_skipsearch=KWPDIAG_FALSE;
    kwpdiag_programmingrequest=KWPDIAG_FALSE; 
    kwpdiag_rcrrpruning=KWPDIAG_FALSE;
}
/* ===========================================================================
//
// Name:            kwpdiag_periodic
//
// Description:     Periodic task for KWP Diag processing
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/

void kwpdiag_periodic(void)
{
    signed char index;
   // kwpdiagu8 current_sess;
    kwpdiagu8 rcrp_limit = KWPDIAG_FALSE;
    /*kwpdiag_posthandler_t posthandler;*/
    kwpdiagu8 ret;
    kwpdiagu8 ccr;
	   
    for(index=0;index<KWPDIAG_NUMREQTYPE;index++)
    {
        if(kwpdiag_buffcntrl[index].confirmation_rxed == KWPDIAG_TRUE)
        {
            ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
            kwpdiag_buffcntrl[index].confirmation_rxed = KWPDIAG_FALSE;
            KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
            /*posthandler = kwpdiag_srvinfotable[kwpdiag_buffcntrl[index].\
                          currentsrvidx].funchandler.pkwpdiag_posthandle;*/
            if(posthandler != NULL)
            {
                posthandler();   
            }
        }
    }
    ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
    if(GETP3TIMER()!= (kwpdiagu16) 0)					  
    {
        DECP3TIMER();
        if(!GETP3TIMER())
        {
            KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
            kwpdiag_P3timeout();    
        }
        else
        {
            KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
        }    
    }
    else
    {
        KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
    }
    #if(KWPDIAG_RCRRP == KWPDIAG_ENABLE)
    ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
    if(GETRPLIMITER()!= (kwpdiagu16)0)
    {
        DECRPLIMITER();
        if(!GETRPLIMITER())
        {
            STOPP2TIMER();
            current_sess = kwpdiag_currsess;
            kwpdiag_init();
            /*added to avoid session change due to Init*/
            kwpdiag_currsess = current_sess;
            KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
            ApplDiagServiceTimeout();
            rcrp_limit=KWPDIAG_TRUE;                
        }
        else
        {
            KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
        }    
    }
    else
    {
        KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
    }
    #endif
    if(rcrp_limit == KWPDIAG_FALSE)
    {
        #if(KWPDIAG_RCRRP == KWPDIAG_ENABLE)
        ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
        if(GETP2TIMER() != (kwpdiagu16)0)
        {
            DECP2TIMER();
            if(!GETP2TIMER())
            {
                KWPDIAG_EXIT_CRITICAL_SECTION(ccr);          
                kwpdiag_P2timeout();    
            }
            else
            {
                KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
            }    
        }
        else
        {
            KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
        }
        #else
        ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
        if(GETP2TIMER() != (kwpdiagu16)0)
        {
            DECP2TIMER();
            if(!GETP2TIMER())
            {
                KWPDIAG_EXIT_CRITICAL_SECTION(ccr);          
                ApplDiagServiceTimeout();    
            }
            else
            {
                KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
            }    
        }
        else
        {
            KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
        }
        #endif
        for(index=(KWPDIAG_NUMREQTYPE-1);index>=0;index--)
        {
            if(kwpdiag_buffcntrl[index].ind_flag == KWPDIAG_TRUE)
            {
                STOPP3TIMER();
                ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
                kwpdiag_buffcntrl[index].ind_flag = KWPDIAG_FALSE;
                KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
                kwpdiag_dispatcher((unsigned char)index);
            }
            else
            {
            }
        }
        if(kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].tx_ready == KWPDIAG_TRUE)
        {
            ret = KWPDIAG_TP_TRANSMIT(KWPDIAG_TPCHANNEL,
                             pkwpdiagbuffer[KWPDIAG_PRIMARYREQ],
                             kwpdiag_bufflen[KWPDIAG_PRIMARYREQ]);
            if(ret == VTP_SUCCESS)
            {
                ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
                kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].tx_ready = KWPDIAG_FALSE;
                KWPDIAG_EXIT_CRITICAL_SECTION(ccr);   
            }
            else
            {
            }
        }
        else
        {
        }
    }    
}
/* ===========================================================================
//
// Name:            kwpdiag_functionalgetbuffer
//
// Description:     TP Interface to get the functional reception buffer.
//
// Inputs:          datalength of the request
//
// Returns:         pointer to the functional request buffer.
//
// =========================================================================*/

/*TP Layer Interfaces*/
kwpdiagu8* kwpdiag_functionalgetbuffer (kwpdiagu16 dataLength)
{
    kwpdiagu8* pbuffer;

	kwpdiag_currreq = KWPDIAG_SECONDARYREQ;

    if((kwpdiag_buffcntrl[KWPDIAG_SECONDARYREQ].bufferlocked == KWPDIAG_FALSE) &&\
       (kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].bufferlocked == KWPDIAG_FALSE))
    {
        if(dataLength > (kwpdiagu16)KWPDIAG_MAXSECONDARYBUFFLEN )
        {
            VTP_SetFC_Overflow();
            pbuffer= NULL;
        }
        else
        {
            kwpdiag_buffcntrl[KWPDIAG_SECONDARYREQ].bufferlocked = KWPDIAG_TRUE;
            pbuffer=(pkwpdiagbuffer[KWPDIAG_SECONDARYREQ]);
        }
    }
    else
    {
        pbuffer= NULL;
    }
    return pbuffer;
}
/* ===========================================================================
//
// Name:            kwpdiag_physicalgetbuffer
//
// Description:     TP Interface to get the Physical reception buffer.
//
// Inputs:          datalength of the request
//
// Returns:         pointer to the physical request buffer.
//
// =========================================================================*/
kwpdiagu8* kwpdiag_physicalgetbuffer(kwpdiagu16 dataLength)
{
   
    kwpdiagu8* pbuffer;

	kwpdiag_currreq = KWPDIAG_PRIMARYREQ;

    if((kwpdiag_buffcntrl[KWPDIAG_SECONDARYREQ].bufferlocked == KWPDIAG_FALSE) &&\
       (kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].bufferlocked == KWPDIAG_FALSE))
    {
        if(dataLength > (kwpdiagu16)KWPDIAG_MAXPRIMARYBUFFLEN )
        {
            VTP_SetFC_Overflow();
            pbuffer= NULL;
        }
        else
        {
            kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].bufferlocked = KWPDIAG_TRUE;
            pbuffer=(pkwpdiagbuffer[KWPDIAG_PRIMARYREQ]);
        }
    }
    else
    {
        pbuffer= NULL;
    } 
    return pbuffer;   
}
/* ===========================================================================
//
// Name:            kwpdiag_physicalreqind
//
// Description:     TP Interface to indicate Reception of physical message
//
// Inputs:          datalength of the request
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_physicalreqind(kwpdiagu16 dataLen)
{
    STARTP2TIMER();
    kwpdiag_bufferror[KWPDIAG_PRIMARYREQ] = KWPDIAG_NOERROR;
    kwpdiag_bufflen[KWPDIAG_PRIMARYREQ] = dataLen; 
    kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].ind_flag = KWPDIAG_TRUE;
    kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].makeresponse = KWPDIAG_TRUE;
    kwpdiag_currreq = KWPDIAG_PRIMARYREQ;
    TpRxResetChannel(0);
}
/* ===========================================================================
//
// Name:            kwpdiag_functionalreqind
//
// Description:     TP Interface to indicate Reception of functional message 
//
// Inputs:          datalength of the request
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_functionalreqind(kwpdiagu16 dataLength)
{
    kwpdiag_bufferror[KWPDIAG_SECONDARYREQ] = KWPDIAG_NOERROR;
    kwpdiag_bufflen[KWPDIAG_SECONDARYREQ] = dataLength; 
    kwpdiag_buffcntrl[KWPDIAG_SECONDARYREQ].ind_flag = KWPDIAG_TRUE;
    kwpdiag_currreq = KWPDIAG_SECONDARYREQ;
    TpFuncResetChannel();
}
/* ===========================================================================
//
// Name:            kwpdiag_confirmation
//
// Description:     TP Interface to confirm Message Transmission
//
// Inputs:          
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_confirmation(kwpdiagu8 state)
{
    if(kwpdiag_rcrrpruning == KWPDIAG_TRUE)
    {        
        kwpdiag_rcrrpruning=KWPDIAG_FALSE; 
        if(kwpdiag_programmingrequest == KWPDIAG_TRUE)
        {
            kwpdiag_programmingrequest=KWPDIAG_FALSE;
            #if defined( FBL_ENABLE_FBL_START )
            CallFblStart();
            #endif
        }
        else
        {
        }
    }
    else
    {
        kwpdiag_postprocessing(kwpdiag_currreq,state);
    }
}
/* ===========================================================================
//
// Name:            kwpdiag_txerrorind
//
// Description:     TP Interface to indicate Transmission Error
//
// Inputs:          
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_txerrorind(kwpdiagu8 state)
{
    TpRxResetChannel(0);
    kwpdiag_confirmation(state);
}

/* ===========================================================================
//
// Name:            kwpdiag_rxerrorind
//
// Description:     TP Interface to indicate Reception Error
//
// Inputs:          
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_rxerrorind(kwpdiagu8 state)
{
    kwpdiagu8 ccr;
  
    TpRxResetChannel(0);
    STARTP3TIMER();
    ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
    kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].bufferlocked = KWPDIAG_FALSE;
    KWPDIAG_EXIT_CRITICAL_SECTION(ccr);

    /*if(state == VTP_ERROR)
    {
        if(kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].currentsrvidx != 0xFF)
        {
            STARTP3TIMER();
            sid = kwpdiag_srvinfotable[kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].\
                              currentsrvidx].requestsid;
            kwpdiag_sendnegativeresponse(sid,KWPDIAG_NRCTRANSFERABORTED);
        }
    }
    else
    {
    }*/
}

/* ===========================================================================
//
// Name:            kwpdiag_postprocessing
//
// Description:     
//
// Inputs:          
//
// Returns:         none
//
// =========================================================================*/

void kwpdiag_postprocessing(unsigned char bufferindex,unsigned char respdata) 
{
    kwpdiagu8 ccr;
    if(kwpdiag_bufferror[bufferindex] == KWPDIAG_NOERROR)
    {
        if(respdata == VTP_SUCCESS)
        {
            kwpdiag_buffcntrl[bufferindex].confirmation_rxed = KWPDIAG_TRUE;
        }
        else
        {
        }
    }
    STARTP3TIMER();
    if(bufferindex == KWPDIAG_PRIMARYREQ)
    {
        STOPP2TIMER();           
    }
    ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
    kwpdiag_buffcntrl[bufferindex].bufferlocked = KWPDIAG_FALSE;
    KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
}
/* ===========================================================================
//
// Name:            kwpdiag_P2timeout
//
// Description:     P2 Timeout handling to send RCRRP
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_P2timeout(void)
{    
    kwpdiagu8 ret;
    ret = kwpdiag_sendrcrrp();
    if ( ret != KWPDIAG_TRUE)
    {
        kwpdiag_rcrrpruning=KWPDIAG_FALSE;
        INCP2TIMER();
    }
    else
    {
    }    
}
/* ===========================================================================
//
// Name:            kwpdiag_sendrcrrp
//
// Description:     send RCRRP
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
kwpdiagu8 kwpdiag_sendrcrrp (void)
{
 
    kwpdiagu8 ret = KWPDIAG_FALSE;
    kwpdiag_rcrpbuffer[0]=KWPDIAG_NEGRESPCODE;
    kwpdiag_rcrpbuffer[1]=kwpdiag_primarybuffer[0];
    kwpdiag_rcrpbuffer[2]=KWPDIAG_RCRRPRESPONSE;
    ret = KWPDIAG_TP_TRANSMIT(KWPDIAG_TPCHANNEL, kwpdiag_rcrpbuffer, 3);
    if(ret == VTP_SUCCESS )
    {
        kwpdiag_rcrrpruning=KWPDIAG_TRUE;
        STARTP2TIMERRCRRPMAX();
        ret = KWPDIAG_TRUE;
    }
    else
    {
        ret = KWPDIAG_FALSE;
    }
    return (ret);
}
/* ===========================================================================
//
// Name:            kwpdiag_P3timeout
//
// Description:     Diag Session Time out Handling
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_P3timeout(void)
{
    STOPP2TIMER();
    if(kwpdiag_currsess != DEFAULTSESSION)
	{
    	ApplDiagSessionTimeout();   /* Included for giving session timeout to diag */
		UPDATE_SESSION(DEFAULTSESSION);
	}
}
/* ===========================================================================
//
// Name:            kwpdiag_dispatcher
//
// Description:     Diagnostic Processing
//
// Inputs:          Type of Message Buffer
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_dispatcher(unsigned char bufferidx)
{
    kwpdiagu16 len;
    kwpdiagu8 *msgptr;
    kwpdiagu8 index ,match = FALSE;
    /*kwpdiag_mainhandler_t mainhandler;*/
    #if (KWPDIAG_RESPONSE_ON_FUNC_REQ == KWPDIAG_ENABLE)
    kwpdiagu8 ccr;
    #endif
    static kwpdiagu8 srv_index=0xFF;

    len=kwpdiag_bufflen[bufferidx];
    msgptr=pkwpdiagbuffer[bufferidx];

    if( len == (kwpdiagu16)0)
    {
        kwpdiag_bufferror[bufferidx]=KWPDIAG_NRCSRVNOTSUPPORTED;
        kwpdiag_buffcntrl[bufferidx].makeresponse=KWPDIAG_FALSE;
        kwpdiag_processingdone(bufferidx,VTP_SUCCESS);
    }
    else
    {
        if(kwpdiag_skipsearch == KWPDIAG_FALSE)
        {
            for(index=0;(index<kwpdiag_numsrv) && (!match);index++)
            {
                if(msgptr[0]==kwpdiag_srvinfotable[index].requestsid)
                {
                    match = TRUE;
                    if (match != FALSE)
                    {
                        /*We need to match if the current session & the SID
                         session supported are matching to proceed further!*/
                        if((KWPDIAG_SECONDARYREQ == kwpdiag_currreq)\
							&&( DIAG_NULL_SESS == kwpdiag_srvinfotable[index].fun_sesssupp ))
                        {
							kwpdiag_bufferror[bufferidx]=KWPDIAG_NRCSRVNOTSUPPORTED;
						}
                        else if (((KWPDIAG_PRIMARYREQ == kwpdiag_currreq)&&((kwpdiag_currsess & kwpdiag_srvinfotable[index].phy_sesssupp) != FALSE))\
							||((KWPDIAG_SECONDARYREQ == kwpdiag_currreq)&&((kwpdiag_currsess & kwpdiag_srvinfotable[index].fun_sesssupp) != FALSE)))
                        {
                            kwpdiag_buffcntrl[bufferidx].currentsrvidx=index;
                            srv_index = index;
                        }
                        else
                        {
                            /*The session supported doesnt match with Curr Sess*/
                            /*Update the NRC accordingly.*/
                            kwpdiag_bufferror[bufferidx]=KWPDIAG_NRCNOTSUPPORTEDINACTIVESESS;
                        }
                    }
                    else
                    {
                        /*Either the SID or the SUBFUNCTION Didnt match!*/
                    }
                }
                else
                {}
            }

            if(srv_index == 0xFF)
            {
				#if 1
				if(bufferidx == KWPDIAG_SECONDARYREQ)
				{
					/* send NRC 0x7F */
					if(kwpdiag_bufferror[KWPDIAG_SECONDARYREQ] == KWPDIAG_NRCNOTSUPPORTEDINACTIVESESS)
					{
						if(kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].bufferlocked == KWPDIAG_FALSE)
						{
							(void)memcpy(pkwpdiagbuffer[KWPDIAG_PRIMARYREQ],pkwpdiagbuffer[KWPDIAG_SECONDARYREQ],len);
							ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
							kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].currentsrvidx=kwpdiag_buffcntrl[KWPDIAG_SECONDARYREQ].currentsrvidx;
							kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].bufferlocked = KWPDIAG_TRUE;
							kwpdiag_buffcntrl[KWPDIAG_SECONDARYREQ].bufferlocked = KWPDIAG_FALSE;
							kwpdiag_physicalreqind(len);
							KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
							kwpdiag_skipsearch = KWPDIAG_TRUE;
							msgptr = pkwpdiagbuffer[KWPDIAG_PRIMARYREQ];
							kwpdiag_bufferror[KWPDIAG_PRIMARYREQ] = kwpdiag_bufferror[KWPDIAG_SECONDARYREQ];


							/*Update the negative response to the tester*/
							if(kwpdiag_bufferror[KWPDIAG_PRIMARYREQ] == KWPDIAG_NOERROR)
							{
								kwpdiag_bufferror[KWPDIAG_PRIMARYREQ] = KWPDIAG_NRCSRVNOTSUPPORTED;
							}
							kwpdiag_processingdone(KWPDIAG_PRIMARYREQ,0);
						}
					}
					/* ignor NRC 0x11 */
					else
					{
						kwpdiag_postprocessing(KWPDIAG_SECONDARYREQ,VTP_FALSE);
					}
				}
				else
				{
				/*Update the negative response to the tester*/
				if(kwpdiag_bufferror[KWPDIAG_PRIMARYREQ] == KWPDIAG_NOERROR)
				{
					kwpdiag_bufferror[KWPDIAG_PRIMARYREQ] = KWPDIAG_NRCSRVNOTSUPPORTED;
				}
				kwpdiag_processingdone(KWPDIAG_PRIMARYREQ,0);
				}
				#else
				{
				   if(bufferidx == KWPDIAG_SECONDARYREQ)
				   {
					   kwpdiag_postprocessing(KWPDIAG_SECONDARYREQ,VTP_SUCCESS);
				   }
				   else
				   {
					   /*Update the negative response to the tester*/
					   if(kwpdiag_bufferror[KWPDIAG_PRIMARYREQ] == KWPDIAG_NOERROR)
					   {
						   kwpdiag_bufferror[KWPDIAG_PRIMARYREQ]=KWPDIAG_NRCSRVNOTSUPPORTED;
					   }
					   kwpdiag_processingdone(KWPDIAG_PRIMARYREQ,0);
				   }
				}
				#endif
				
            }
            else
            {
                if(bufferidx == KWPDIAG_SECONDARYREQ)
                {
                    #if (KWPDIAG_RESPONSE_ON_FUNC_REQ == KWPDIAG_ENABLE)
                    #if (KWPDIAG_RESPONSE_ALL_ON_FUNC_REQ == KWPDIAG_DISABLE)
                    if(KWPDIAG_ENABLE == kwpdiag_srvinfotable[kwpdiag_buffcntrl[bufferidx].currentsrvidx].responsefuncreq)
                    #endif
                    {
                        if(kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].bufferlocked == KWPDIAG_FALSE)
                        {
                            (void)memcpy(pkwpdiagbuffer[KWPDIAG_PRIMARYREQ],
                            pkwpdiagbuffer[KWPDIAG_SECONDARYREQ],
                            len);
                            ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
                            kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].currentsrvidx=kwpdiag_buffcntrl[KWPDIAG_SECONDARYREQ].currentsrvidx;
                            kwpdiag_buffcntrl[KWPDIAG_PRIMARYREQ].bufferlocked = KWPDIAG_TRUE;
                            kwpdiag_buffcntrl[KWPDIAG_SECONDARYREQ].bufferlocked = KWPDIAG_FALSE;
                            kwpdiag_physicalreqind(len);
							kwpdiag_actul_req = KWPDIAG_SECONDARYREQ;
                            KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
                            kwpdiag_skipsearch = KWPDIAG_TRUE;
							msgptr = pkwpdiagbuffer[KWPDIAG_PRIMARYREQ];
                        }
                        else
                        {
                                /*to be identified*/
                        }
                    }
                    #endif
                }
				else
				{
					kwpdiag_actul_req = KWPDIAG_PRIMARYREQ;
				}
            }
        }
        else
        {
            kwpdiag_skipsearch = KWPDIAG_FALSE;        
        }
		
        if (srv_index != 0xFF)
        {
            /*mainhandler = *(kwpdiag_srvinfotable[srv_index].funchandler.pkwpdiag_mainfunc);*/
            mainhandler((msgptr+1),(len-(kwpdiagu16)1));  
            srv_index=0xFF;
        }
        else
        {
        }
    }
}
/* ===========================================================================
//
// Name:            kwpdiag_testerpresenthandler
//
// Description:     Tester Present handling 
//
// Inputs:          Length of Diagnostic Message
//
// Returns:         none
//
// =========================================================================*/

void kwpdiag_testerpresenthandler(unsigned char *pdiagdata,unsigned short diagrequestlength)
{
   // kwpdiagu8 ccr;
    if(diagrequestlength < (kwpdiagu16)1)
    {
        kwpdiag_bufferror[KWPDIAG_PRIMARYREQ]=KWPDIAG_NRCINVALIDFORMAT;
		kwpdiag_processingdone(KWPDIAG_PRIMARYREQ,0);
	}
	else
	{
        switch(pdiagdata[0])
        {
	        case 0x00:
				if(diagrequestlength > (kwpdiagu16)1)
				{
					kwpdiag_bufferror[KWPDIAG_PRIMARYREQ]=KWPDIAG_NRCINVALIDFORMAT;
					kwpdiag_processingdone(KWPDIAG_PRIMARYREQ,0);
				}
				else
				{
					kwpdiag_processingdone(KWPDIAG_PRIMARYREQ,1);
				}
				
	            break;
				
	        case 0x80:
				if(diagrequestlength > (kwpdiagu16)1)
				{
					kwpdiag_bufferror[KWPDIAG_PRIMARYREQ]=KWPDIAG_NRCINVALIDFORMAT;
					kwpdiag_processingdone(KWPDIAG_PRIMARYREQ,0);
				}
				else
				{
					kwpdiag_postprocessing(KWPDIAG_PRIMARYREQ,VTP_SUCCESS);
				}
				
	            break;
				
	        default:
				if(( KWPDIAG_PRIMARYREQ == kwpdiag_actul_req )||( KWPDIAG_SECONDARYREQ == kwpdiag_actul_req ))
				{
					kwpdiag_bufferror[KWPDIAG_PRIMARYREQ]=KWPDIAG_NRCSUBFUNCNOTSUPPORTED;
					kwpdiag_processingdone(KWPDIAG_PRIMARYREQ,0);
				}
				else
				{
					kwpdiag_postprocessing(KWPDIAG_PRIMARYREQ,VTP_SUCCESS);
				}
	            break;
        
		}
	}
}

/* ===========================================================================
//
// Name:            kwpdiag_setnegativeresponse
//
// Description:     Handling of Error Code
//
// Inputs:          Error Code
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_setnegativeresponse(unsigned char bufferindex,unsigned char errorcode)
{
    kwpdiag_bufferror[bufferindex] = errorcode; 
}
/* ===========================================================================
//
// Name:            kwpdiag_processingdone
//
// Description:     Response Processing
//
// Inputs:          Response Length
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_processingdone(unsigned char bufferindex,unsigned short reslen )
{
    kwpdiagu8 *msgptr;
    kwpdiagu8 ccr;
    msgptr=pkwpdiagbuffer[bufferindex];
    if(kwpdiag_buffcntrl[bufferindex].makeresponse == KWPDIAG_TRUE)
    {
        STOPP2TIMER();
        ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
        kwpdiag_buffcntrl[bufferindex].tx_ready = 1;   
        KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
        if(kwpdiag_bufferror[bufferindex] != KWPDIAG_NOERROR)
        {
            msgptr[1]=msgptr[0];
            msgptr[0]=KWPDIAG_NEGRESPCODE;
            msgptr[2]=kwpdiag_bufferror[bufferindex];
            kwpdiag_bufflen[bufferindex]=KWPDIAG_NEGRESPLEN;
        }
        else
        {
            msgptr[0]=kwpdiag_srvinfotable[kwpdiag_buffcntrl[bufferindex].\
                          currentsrvidx].responsesid;
            kwpdiag_bufflen[bufferindex]=reslen+1;
        }

		/* unlock receive buffer */
		ccr = KWPDIAG_ENTER_CRITICAL_SECTION();
		kwpdiag_buffcntrl[bufferindex].bufferlocked = KWPDIAG_FALSE;
		kwpdiag_buffcntrl[bufferindex].makeresponse = KWPDIAG_FALSE;
		KWPDIAG_EXIT_CRITICAL_SECTION(ccr);
    }
    else
    {
        kwpdiag_postprocessing(bufferindex,VTP_SUCCESS);
    }
}
/* ===========================================================================
//
// Name:            kwpdiag_stopp3timer
//
// Description:     Stop the P3 Timer
//
// Inputs:          Channel
//
// Returns:         none
//
// =========================================================================*/

/*void kwpdiag_stopp3timer(kwpdiagu8 channel)
{
    STOPP3TIMER();
}*/
/* ===========================================================================
//
// Name:            kwpdiag_sendnegativeresponse
//
// Description:     Sending Negative Response
//
// Inputs:          SID,Error code
//
// Returns:         none
//
// =========================================================================*/

void kwpdiag_sendnegativeresponse(unsigned char sid,unsigned char errorcode)
{
    kwpdiag_negresp[0]=0x7f;
    kwpdiag_negresp[1]=sid;
    kwpdiag_negresp[2]=errorcode;
    (void)KWPDIAG_TP_TRANSMIT(KWPDIAG_TPCHANNEL, kwpdiag_negresp, 3);
}

 /* ===========================================================================
//
// Name:            kwpdiag_FuncNotSupported
//
// Description:     Sending Negative Response
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/

void kwpdiag_FuncNotSupported(unsigned char *pdiagdata,unsigned short diagrequestlength)
{
    kwpdiag_bufferror[KWPDIAG_PRIMARYREQ]=KWPDIAG_SRVNOTSUPPORTEDINACTIVEDIAGMODE;
    kwpdiag_processingdone(KWPDIAG_PRIMARYREQ,0);
}


kwpdiagu8 kwpdiag_DiagSession(void)
{
	return kwpdiag_currsess;
}


UINT8 kwpdiag_enter_crtical(void)
{
	__disable_irq();
	return 1;
}

UINT8 kwpdiag_exit_crtical(UINT8 ccr)
{
	(void)ccr;
	__enable_irq();
	return 1;
}

