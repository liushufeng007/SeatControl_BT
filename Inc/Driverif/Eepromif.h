
#ifndef __EEPROMIF_H__
#define __EEPROMIF_H__
/*!
* @file ac780x_sweeprom.h
*
* @brief This file provides software eeprom integration functions interface.
*
*/
#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "user_init.h"


typedef enum
{
	EEPROM_BANK_MOTOR   = 0,
	EEPROM_BANK_APP     = 1,
	EEPROM_BANK_DIAG    = 2,
	EEPROM_BANK_FBL     = 3,
	EEPROM_BANK_NUMBER  = 4
}Eeprom_Bank_ID_e;



#define SWEEPROM_SIZE_MOTOR   (0x200UL)
#define SWEEPROM_SIZE_APP     (0x200UL)
#define SWEEPROM_SIZE_DIAG    (0x200UL)
#define SWEEPROM_SIZE_FBL     (0x200UL)


#define SWEEPROM_START_SECTOR_ID_MOTOR   (4UL)
#define SWEEPROM_START_SECTOR_ID_APP     (12UL)
#define SWEEPROM_START_SECTOR_ID_DIAG    (20UL)
#define SWEEPROM_START_SECTOR_ID_FBL     (28UL)



/* ============================================  Define  ============================================ */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief software eeprom initialize,actual available space are (Size - 2)
 *
 * @param[in] size: config software eeprom size(half word)
 * @param[in] pageIndex: the start page index in eflash occupied by software eeprom
 * @return none
 */
void SWEEPROM_Init(uint16_t size, uint32_t pageIndex,uint8_t bankId);

/*!
 * @brief software eeprom Write
 *
 * @param[in] writeAddr: software eeprom address
 * @param[in] dataBuffer: data buffer pointer
 * @param[in] writeNum: write half word number
 * @return statusRes: software eeprom write status
 */
FL_ErrorStatus SWEEPROM_Write(uint16_t writeAddr, const uint16_t *dataBuffer, uint16_t writeNum,uint8_t bankId);

/*!
 * @brief software eeprom read
 *
 * @param[in] readAddr: Read start address of software software eeprom
 * @param[in] dataBuffer: Point to the buffer ready to read the data from software eeorom
 * @param[in] readNum: read half word number
 * @return software eeprom read status
 */
FL_ErrorStatus SWEEPROM_Read(uint16_t readAddr, uint16_t *dataBuffer, uint16_t readNum,uint8_t bankId);

/*!
 * @brief software eeprom erase(include backup area)
 *
 * @param[in] none
 * @return statusRes: software eeprom erase status
 */
FL_ErrorStatus SWEEPROM_Erase(uint8_t bankId);

#ifdef __cplusplus
}
#endif

#endif /* _AC780X_SWEEPROM_H */

/* =============================================  EOF  ============================================== */
