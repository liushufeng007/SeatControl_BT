#if !defined(KWPDIAG_PAR_H)
#define KWPDIAG_PAR_H

#include "kwpdiag_cfg.h"

extern void ApplDiagStartDiagnosticSession(unsigned char *pdiagdata,unsigned short diagrequestlength);
extern void ApplDiagEcuReset(unsigned char *pdiagdata,unsigned short diagrequestlength);
extern void ApplDiagReadDTCInformation(unsigned char *pdiagdata,unsigned short diagrequestlength);
extern void ApplDiagClearDiagnosticInformation(unsigned char *pdiagdata,unsigned short diagrequestlength);
extern void ApplDiagInputOutputControlByLocalId(unsigned char *pdiagdata,unsigned short diagrequestlength);
extern void ApplDiagStartRoutineByLocaldentifier(unsigned char *pdiagdata,unsigned short diagrequestlength);

extern void ApplDiagReadDataByCommonId(unsigned char *pdiagdata,unsigned short diagrequestlength);
extern void ApplDiagWriteDataByCommonId(unsigned char *pdiagdata,unsigned short diagrequestlength);

extern void ApplDiagStartDiagnosticSessionPosthandler(void);
extern void ApplDiagEcuResetPosthandler(void);

extern void ApplDiag_ControlDTCSetting(unsigned char *pdiagdata,unsigned short diagrequestlength);

extern void ApplDiag_Read_Memory_by_Address(unsigned char *pdiagdata,unsigned short diagrequestlength);
extern void ApplDiagSecurityAccess(unsigned char *pdiagdata,unsigned short diagrequestlength);
extern void ApplDiag_CommunicatinControl(unsigned char *pdiagdata,unsigned short diagrequestlength);
extern void ApplDiag_Write_Memory_by_Address(unsigned char *pdiagdata,unsigned short diagrequestlength);


extern void ApplDiagSessionTimeout(void);
extern void ApplDiagServiceTimeout(void);

extern void kwpdiag_SessionHandler(unsigned char* c, unsigned short s);

#endif /* KWPDIAG_H */


