;Libreria para eleiminar rebotes de las entradas del puerto A
;funciona para entradas con resistencias de Pull-up
;sin invertir previamente
;se debe cargar en la variable "pin" el bit del pin que se quiere eliminar el rebote

	CBLOCK
pin
	ENDC

Damp
		call Retardo_20ms
		call Retardo_10ms	;realizamos un retardo de 30 ms
		movf pin,W
apreta2		
		btfsc PORTA,W 		;comprueba si todavia esta presionado en boton
		goto yasalto 		;si lo solto pues va al siguiente paso
		goto apreta2		;salta a la etiqeuta apreta2
yasalto
		call Retardo_20ms	;
		call Retardo_10ms	;realiza un retardo de 30 ms


