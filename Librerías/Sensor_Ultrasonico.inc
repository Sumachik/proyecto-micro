CBLOCK
waitForEcho
distancia

unidades
decenas
centenas

dividendo
divisor
div
aux

DH
DL
ENDC

#define trigger TRISA,3	;salida del trigger para el sensor
#define echo TRISA,4	;entrada de datos eco del sensor

#define	Carry	STATUS,C
#define Zero 	STATUS,Z

multip	EQU b'10001' ;17		constante que se usará para los cálculos en la subrutina de multiplicación 
diviso	EQU b'1010'  ;10		constante que se usará para los cálculos en la subrutina de división 	

Sensor_Inicializa
      bsf STATUS,RP0			;accedemos al banco 1 de memoria
      
      bcf trigger			;pin trigger como salida
      bsf echo				;pin echo como entrada
      
      bcf STATUS,RP0			;regresamos al banco 0 de memoria
      return				;retorno de la subrutina
      
Lanzar_Disparo
   ;lanzamos la señal de disparo del sensor
   clrf waitForEcho			;variable acumuladora de la cantidad de tiempo
   clrf distancia			;variable en la que se almacenará el cálculo final de la distancia
   clrf unidades			;unidades bcd de distancia	
   clrf decenas				;decenas bcd de distancia
   clrf centenas			;centenas bcd de distancia
   
   bsf trigger				;se lanza el pulso en pin trigger (disparo)
   call Retardo_10micros		;retardo de 10 microsegundos que dura el disparo
   bcf trigger				;apagamos pin trigger
Esperar_Echo
   ;revisamos el pin Echo
   btfss echo				;echo = 1?
   goto Esperar_Echo			;no, siga esperando
Leer_Echo
   ;una vez Echo = 1, esperamos a que se ponga a 0, el tiempo que este se tarde
   ;va directamente relacionado con la distancia a la que se encuentra el objeto
   call Retardo_100micros		;se esperan 100 microsegundos osea 1x10^-4 segundos
   goto Inc_Wait			;salto a etiqueta que incrementa la variable de espera de tiempo
Continue 
   btfsc echo				;echo = 0?
   goto Leer_Echo			;no, siga esperando
;Calcular Distancia
   ;una vez que Echo = 0 significa que la señal enviada ya regresó al sensor, el tiempo que esta se tardó
   ;está acumulado en la variable waitForEcho, en donde cada unidad equivale a 100 microsegundos
   ;Para calcular la distancia tomamos en cuenta la velocidad del sonido a través del aire
   ;que es de 34000 cm/s
   ;Si distancia es la distancia entre el objeto y el sensor entonces
   ;La distancia total recorrida por la señal es el doble de la distancia que necesitamos, 
   ;ya que el recorrido fue de ida y regreso
   ;Por lo que d = (waitForEcho segundos * 34000 cm/segundos)/(2*10000) = (waitForEcho * 17 / 10) cm
   ; 17 decimal = 10001 binario se usará para la subrutina multiplica
   ; 10 decimal = 1010 binario se usará para la subrutina divide
   ;es por esto que se definieron estas constantes al inicio de la librería
Finish_Read
   call    Multiplica              ;subrutina que suma el multiplicando, multiplicador veces
   call    Divide			;subrutina que le resta el divisor al dividendo, y cuenta las veces que es posible
					  ;hacerlo, este número es el cociente
   movf distancia,W			;W = distancia = waitForEcho * 17 / 10 operado anteriormente
   call BIN_a_BCD			;convertimos distancia a bcd
   
   movf BCD_Centenas,W			;W = BCD_Centenas
   movwf centenas			;centenas = BCD_Centenas
   
   movf BCD_Decenas,W			;W = BCD_Decenas
   movwf decenas			;decenas = BCD_Decenas
   
   movf BCD_Unidades,W			;W = BCD_Unidades
   movwf unidades			;unidades = BCD_Unidades   
   return  
   
   
Inc_Wait
   movlw .127				;W = 127, cuando waitForEcho == 127, distancia = 254 cm aproximadamente, valor máximo
					  ;a representar
   subwf waitForEcho,W			;restamos waitForEcho - 127, para ver si llegó a este valor
   btfsc Zero				;revisamos flag Z, el resultado de la resta dio cero?
   goto Finish_Read			;si, entonces se finaliza la lectura de distancia
   incf waitForEcho,F			;no, entonces incrementamos variable acumuladora de cantidad de tiempo
					  ;en la cual cada unidad equivale a 100 microsegundos
   goto Continue			;salto a etiqueta continue
   
   
Multiplica
    clrf    DH				;variable de nible alto
    clrf    DL				;variable de nible bajo
    movf     multip,W         		;W = multiplicador = 17
    btfsc    Zero                	;Salta si Z=1
    goto    Finish             		;Z=0 multiplicador=0
Bucle
    movf    waitForEcho,W               ;W=waitForEcho
    addwf    waitForEcho,W         	;W += multiplicando
    movwf    DL                      	;DL=W
    btfsc    Carry               	;Salta si C=0, Carry es igual a cero?
    incf    DH,F			;no, incrementamos nible alto
Resto
    decfsz    multip,F        		;multiplicador-1
    goto    Bucle                   	;no hemos acabado, el ciclo se repite hasta que el multiplicador es cero
Finish
    swapf DH,W				;invertimos bits del nible bajo de DH al nible alto de W
    addwf DL,W				;le sumamos W a DL, que poseía los bits del nible bajo
    movwf distancia			;guardamos esta suma en variable distancia
    return                              ;fin subrutina
    
    
Divide
   clrf div			;variable que acumula el cociente
   clrf aux			;auxiliar temporal para realizar los cálculos
   clrf dividendo		;variable a almacenar dividendo
   clrf divisor			;variable a almacenar divisor
   
   movf distancia,W		;W = distancia
   movwf dividendo		;dividendo = distancia
   movf diviso,W		;W = diviso = 10
   movwf divisor		;divisor = diviso

   movf dividendo,W      ; copia dividendo a aux, para no modificar dividendo
   movwf aux		;aux = W
Divide1
   movf divisor,W           ;W = divisor
   subwf aux,F		; resta aux-divisor y lo almacena en aux
   incf div,F  		;div = div + 1
   movf div,W       	; compara divisor > aux. W = div
   subwf divisor,W	;W = divisor - div
   btfss Zero		;la resta dio cero?
   goto Divide1		;no, seguir operando
   movf aux,W		;si, W = aux
   movwf distancia	;distancia = aux
   return