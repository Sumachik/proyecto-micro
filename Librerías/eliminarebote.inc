eliminareb ;subrutina que elimina el rebote
;     	movlw .30
;		call pausawm
		call Retardo_20ms
		call Retardo_10ms
apreta2	btfss PORTA,1 ; chequea si todavia esta presionado en boton
		goto yasalto ; si lo solto pues va al siguiente paso
		goto apreta2
yasalto
;		movlw .30
;		call pausawm
		call Retardo_20ms
		call Retardo_10ms
        return

#include Retardos.inc

