SISTEMA DE AUTENTICACIÓN DE USUARIOS POR PROXIMIDAD BAJO LA PLATAFORMA EZ430-CHRONOS
(Proximity-based Access Control using eZ430-Chornos platform.)

Cliente:	ez430-Chronos
AP:		CC1111+PC
Servidor: 	PC 

Para la comunicación se usara un protocolo especifico para esta tarea. El cual usará criptografía simetrica para la comunicación entre AP y Clienta, y criptografia asimétrica entre AP y Servidor. 


Protocol: we use symmetric cryptography for communication between AP and Client, and asymmetric cryptography between AP and Server. The configuration of server/client/AP is in README file in each folder.



	Server (PC)											AP (CC1111+PC)								 Client (eZ430-Chronos)
	   |													|														|
	   |													|-----------------beacon(RAND1)------------------------>|
	   |													|														|
	   |													|<-------------AESks(uid,AESkw(RAND1))------------------|
	   |													|														|
	   |<------------uid,RAND,AESkw(RAND1)------------------|														|
	   |													|														|
	   |-------------STATUS,AESkw(RAND1+1)----------------->|														|
	   |													|														|
	   |													|-------------------AESkw(RAND+1)---------------------->|
	   |											SESSION	|														|
	   |						 					OPENED	|														|
	   |													|-----------------beacon(RAND2)------------------------>|
	   |													|														|
	   |													|<-------------AESks(uid,AESkw(RAND2))------------------|
	   |													|														|
	   |<------------uid,RAND,AESkw(RAND1)------------------|														|
	   |													|														|
	   |-----------------PING OK--------------------------->|														|
	   |													|														|
	   |													|														|




