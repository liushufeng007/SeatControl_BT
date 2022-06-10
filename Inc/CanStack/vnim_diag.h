#if !defined(VNIMDIAG_CFG_H)
#define VNIMDIAG_CFG_H


#include "kwpdiag.h"

typedef enum
{
    VNTD_NO_RESPONSE_MSGID,
    VNTD_ERROR_RESPONSE_MSGID,
    VNTD_POS_RESPONSE_MSGID
}message_id_type;

void vnim_tx_diag_response(vnim_message_id_type, kwpdiagu8 *, kwpdiagu16, kwpdiagu16);




#endif	/*VNIMDIAG_CFG_H*/



