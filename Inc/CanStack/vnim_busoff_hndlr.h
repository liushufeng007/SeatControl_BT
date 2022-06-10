#if !defined(VNIM_BUSOFF_HNDLR_H)
#define VNIM_BUSOFF_HNDLR_H

extern void vnim_bus_off_hndlr_init (void);

extern void vnim_bus_off_hndlr_periodic (void);

extern void Appl_CAN_busoff_start (void);

extern void Appl_CAN_busoff_end(void);

extern uint8_t vnim_get_busoff_status(void);


#endif /* VNIM_CAN_CFG_H */

/* End of file ============================================================ */
