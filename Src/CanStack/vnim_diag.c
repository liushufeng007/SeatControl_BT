
/* ===========================================================================
//
//  Name:           vnim_diag.c
//
//  Description:    KWP Diagnostics Layer
//
//  Organization:   Multiplex Core Technology
//
// =========================================================================*/

/*****************************************************************************/
/* Include Files                                                             */
/*****************************************************************************/
#include "system.h"
#include <string.h>   					/* necessary for memcpy() */
#include "vnim_diag.h"
#include "kwpdiag.h"
#include "kwpdiag_par.h"
#include "tp_par.h"

/*****************************************************************************/
//DECLARATIONS.
/*****************************************************************************/

/* Diagnostic Transmit Buffer */
#define KWPDIAG_MAXPRIMARYBUFFLEN   290

static kwpdiagu8 Diag_Tx_Data_Buffer[KWPDIAG_MAXPRIMARYBUFFLEN];
static kwpdiagu16 Diag_Tx_Data_Nbytes;

/* Diagnostic Transmit Request Type */
static vnim_message_id_type  Diag_Tx_Message_ID;

static void
vnim_process_diag_tx_signals (void);


/* ===========================================================================
//
// Name:            kwpdiag_SessionHandler
//
// Description:     Update of session states as per the SUBFUNC of SID 10
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/

void kwpdiag_SessionHandler(unsigned char* c, unsigned short s)
{
    unsigned char subfunc ;
    UINT8 last_session;

    last_session = GET_DIAG_SESSION();
	subfunc = (c[0]&(~NORESPONSE_MASK));

    if( 1 == s )	/* check length valid or not */
    {
        switch (subfunc)
        {
            case DEFAULTSUBFUNC:
				{
				 if((last_session == DEFAULTSESSION)
	                   || (last_session == EXTENDEDSESSION))
	                {
	                    UPDATE_SESSION(DEFAULTSESSION);
	                }
					
					break;
            	}
    		case EXTENDEDSUBFUNC:
				{
	                if((last_session == DEFAULTSESSION)
	                   || (last_session == EXTENDEDSESSION))
	                {
	                    UPDATE_SESSION(EXTENDEDSESSION);
	                }
	               
	    			break;
    			}
    		case REPROGSUBFUNC:
				{
	                if(  (last_session == EXTENDEDSESSION))
	                {
	                    if(TRUE == get_diag_addressing())
	                    {            
	    				   UPDATE_SESSION(REPROGRAMMINGSESSION);
	                    }  
	                }

	    			break;
    			}

			default:
				break;
				
        }
    }

    ApplDiagStartDiagnosticSession(c,s);
							  
}
				    

/* ===========================================================================
//
// Name:            kwpdiag_RestHandler
//
// Description:     Update of session states as per the SUBFUNC of SID 11
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void kwpdiag_RestHandler(unsigned char* c, unsigned short s)
{
	ApplDiagEcuReset(c,s);
}
/* ===========================================================================
//
// Name:            vnim_tx_diag_response
//
// Description:     Transmit Diagnostic Request Response
//
//                  Function to issue a response to a Diagnostic request.
//
//                  The byte_length parameter specifies the number of bytes
//                  to transmit for the current response event. If the ring
//                  buffer is not used and all of the response bytes fit
//                  into the diagnostics buffer, the "byte_length" parameter
//                  represents the total number of bytes in the response.
//                  If a ring buffer is used, the response bytes may be
//                  transmitted in "chunks" (requires separate calls to this
//                  function), and the parameter "resp_length" must be
//                  specified for the initial ring buffer response.
//
// Inputs:          message_id:         OS Message ID that Maps to the Diagnostic
//                                      Response Type
//                  bufptr:             Pointer to buffer with the transmit data
//                  byte_length:        Data Length in Bytes (for this packet)
//                  total_resp_length:  Total Response Length in Bytes
//
// Returns:         none
//
// =========================================================================*/
void
vnim_tx_diag_response(vnim_message_id_type message_id, kwpdiagu8 * bufptr,
                      kwpdiagu16 byte_length, kwpdiagu16 total_resp_length)
{
    /* Store the Diagnostic OS Message ID Indicating Response Type */
    Diag_Tx_Message_ID = message_id;

    /* Number of Bytes to Send for this Response "Chunk" */
    Diag_Tx_Data_Nbytes = byte_length;

    /* Put the TP Data in the Buffer */
    (void)memcpy(&Diag_Tx_Data_Buffer, bufptr, Diag_Tx_Data_Nbytes);

    /*Lets send the response immediately.*/
	vnim_process_diag_tx_signals();
}						    


/* ===========================================================================
//
// Name:            vnim_process_diag_tx_signals
//
// Description:     Process Pending Diagnostic Signal Transmit Requests
//
//                  Note this function only gets called if there is a request
//                  to transmit a Diagnostic response that has been issued by
//                  the application.
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
static void
vnim_process_diag_tx_signals (void)
{
    
    switch (Diag_Tx_Message_ID)
    {
    case VNTD_ERROR_RESPONSE_MSGID:

        /* Respond with the Error Code */
        kwpdiag_setnegativeresponse (kwpdiag_currreq,Diag_Tx_Data_Buffer[0]);

        /* Processing is Complete */
        kwpdiag_processingdone(kwpdiag_currreq,1);
		/*We have done with our DIAG RESPONSE TX. RESET the diag TX. msg ID.*/
        Diag_Tx_Message_ID = VNTD_NO_RESPONSE_MSGID;
        break;

    case VNTD_POS_RESPONSE_MSGID:

        //kwpdiag_setpositiveresponse(Diag_Tx_Data_Nbytes,&Diag_Tx_Data_Buffer[0]);
        /* Processing is Complete */
        kwpdiag_processingdone(kwpdiag_currreq,Diag_Tx_Data_Nbytes);
		/*We have done with our DIAG RESPONSE TX. RESET the diag TX. msg ID.*/
        Diag_Tx_Message_ID = VNTD_NO_RESPONSE_MSGID;

        break;
		
	case VNTD_NO_RESPONSE_MSGID:
		kwpdiag_postprocessing(kwpdiag_currreq,VTP_SUCCESS);
		break;

    default:
        break;

    } /* switch */
}

