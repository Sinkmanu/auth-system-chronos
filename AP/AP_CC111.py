#!/usr/bin/python
# Copyright (C) 2012 Manuel Mancera
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
"""
Interfaz de Administracion eZ430-chronos
"""

import serial
import binascii
import eZ430
import time
import os
import random
import struct
import socket
import ssl
import sys
from ConfigParser import SafeConfigParser
import dbus


__author__="Manuel Mancera (sinkmanu@gmail.com)"
__date__ ="$25.11.2012 13:51:21$"
__version__ = "1.0"

#Iniciacion de variables desde el archivo de configuracion
parser = SafeConfigParser()
parser.read('/home/manu/PFC/Proyecto/Server/AP_CC1111/accesspoint.conf') #debera estar en /etc/accesspoint.conf

name_fich="/etc/gdm/custom.conf"
name_fich_aux="/etc/gdm/custom_aux.conf"
file_pam=parser.get('configure_files','file_pam')
error_f=parser.get('configure_files','error_file')
directorio=parser.get('configure_files','path_ap') #path_administrator sera /usr/share/...
icon=parser.get('configure_files','icon')
timeout=parser.get('configure_files','timeout')
login_mode=parser.get('configure_files','login_mode')
mode1_time=parser.get('configure_files','mode1_time')
port_admin=parser.get('server_administrator','port_admin')
server_admin=parser.get('server_administrator','server_admin')

def get_session_dbus(username):
	fich = open("/var/lib/dbus/machine-id","r")
	line = fich.readline()
	line = line.strip('\n')
	fich.close()
	fich = open("/home/"+username+"/.dbus/session-bus/"+line+"-0","r")
	cont = 0
	e = 0
	lines = len(fich.readlines())
	fich.seek(0)
	while cont < lines and e==0:
		cad = fich.readline()
		if "DBUS_SESSION_BUS_ADDRESS" == cad.split("=")[0]:
			session_bus = cad.strip("DBUS_SESSION_BUS_ADDRESS=").strip('\n')
			e = 1
		cont=cont+1
	return session_bus
	fich.close()
	
	

def notificacion_ap(uid,mensaje):
	pid_t = os.fork()
	if pid_t == 0:	
		os.setuid(uid)
		username = get_username(str(uid))
		try:
			bus = dbus.bus.BusConnection(get_session_dbus(str(username)))
			notify_object = bus.get_object('org.freedesktop.Notifications','/org/freedesktop/Notifications')
			notify_interface = dbus.Interface(notify_object,'org.freedesktop.Notifications')
			noti_id = notify_interface.Notify("AccessPoint", 0, "/home/manu/PFC/Proyecto/Server/AP_CC1111/chronos.jpg", "Access Point",mensaje, '',{},10000)

		except dbus.DBusException, err:
			  escribe_error(str(err))
		sys.exit(0)

def escribe_error(file_error):
	fich = open(error_f, "a")
	fich.write(file_error)
	fich.close()

def escribe_fichero(username):
	fich = open(file_pam, "w")
	fich.write(username)
	fich.close()

def resetea_fichero():
	fich = open(file_pam, "w")
	fich.write("")
	fich.close()

def restore_files():
	fich = open(name_fich, "r")
	ficho = open(name_fich_aux, "w")
	cont = 0
	e = 0
	lines = len(fich.readlines())
	fich.seek(0)
	while cont < lines:
		cad = fich.readline()
		if "AutomaticLoginEnable" == cad.split("=")[0]:
			ficho.write("AutomaticLoginEnable=false\n")
		else:
			ficho.write(cad)
		cont=cont+1
	fich.close()
	os.remove(name_fich)
	os.rename(name_fich_aux,name_fich)
	ficho.close()

def write_autologin(username):
	fich = open(name_fich, "r")
	ficho = open(name_fich_aux, "w")
	cont = 0
	e = 0
	lines = len(fich.readlines())
	fich.seek(0)
	while cont < lines:
		cad = fich.readline()
		if "AutomaticLoginEnable" == cad.split("=")[0]:
			ficho.write("AutomaticLoginEnable=true\n")
		else:
			if "AutomaticLogin" == cad.split("=")[0]:
				ficho.write("AutomaticLogin="+username+"\n")
			else:
				ficho.write(cad)
		cont=cont+1
	fich.close()
	os.remove(name_fich)
	os.rename(name_fich_aux,name_fich)
	ficho.close()

def get_username(uid):
	fich = open("/etc/passwd","r")
	cont = 0
	e = 0
	lines = len(fich.readlines())
	fich.seek(0)
	while cont < lines and e==0:
		cad = fich.readline()
		if uid == cad.split(":")[2]:
			e = 1
			name = cad.split(":")[0]
		else:
			cont = cont+1
	fich.close()
	if e == 1:
		return name
	else:
		return 0

def lectura_ACC():
	print "Enviado BM_GET_SIMPLICITIDATA"
	watch.start_simpliciti()
	while 1:
		watch.write("\xFF\x08\x07\x00\x00\x00\x00")
		data = watch.read(7)
		print "X:",ord(data[0])," Y:",ord(data[1])," Z:",ord(data[3])
		time.sleep(0.35)

def lectura_info():
		print "Enviado BM_GET_PRODUCT_ID"
		watch.write("\xFF\x20\x07\x00\x00\x00\x00")
		data = watch.read(7)
		print "%s" % repr(data[3:7])
		print "Enviado BM_GET_STATUS"
		watch.write("\xFF\x00\x04\x00")
		data = watch.read(4)
		print "%s" % repr(data[3])
		watch.close()

def open_graphic_session(uid):
	username = get_username(uid)
	if username != 0:
		if (int(login_mode) == 2):
			write_autologin(username)
			time.sleep(0.2)
			os.system("init 3")
			time.sleep(1)
			os.system("init 5")
			restore_files()
		elif (int(login_mode) == 1):
			escribe_fichero(username)
			time.sleep(int(mode1_time)) # tiempo que tienes para logearte en el sistema
			resetea_fichero()
		notificacion_ap(int(uid),"Logged in to eZ430-Chronos")

def close_graphic_session(uid):
	username = get_username(uid)
	if username != 0:
		#get_session_dbus(username)
		#mejor hacerlo con dbus, aunque tambien hace falta el display
		command_session="/bin/sh /home/manu/PFC/Proyecto/Server/AP_CC1111/close_session.sh "+username
		os.system(command_session)
		
		

def ping_session(id_user):
	contador = 0
	ping_resp = 1

	while (contador < int(timeout)) and ping_resp: 
		try:
			#envio
			n = random.randint(1,999999999)
			command = struct.pack('>3s1i',"\xFF\x74\x07",n)
			watch.write(command)
			watch.flushInput()
			time.sleep(0.4)

			#lectura
			watch.write("\xFF\x75\x15\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
			time.sleep(0.5)
			data = watch.read(21)
			watch.flushOutput()
			cad,uid,rand_aes = struct.unpack('>3sh16s',data)
			if (str(uid) == id_user):
				cadena = struct.pack('>B1ih16s',2,n,uid,rand_aes)
				client = ssl.wrap_socket(socket.socket(),
					ca_certs=str(directorio)+'servercert.pem',
					cert_reqs=ssl.CERT_REQUIRED,
					ssl_version=ssl.PROTOCOL_SSLv23)
				client.connect((server_admin, int(port_admin)))
				client.write(cadena)
				data = client.read()
				status, rand_encrypt = struct.unpack('>h16s',data)
				client.close()
				if status:
					contador=0
					#command_notify="sh /home/manu/PFC/Proyecto/Server/AP_CC1111/APnotify.sh "+get_username(id_user)
					#os.system(command_notify)
					notificacion_ap(int(uid),"Ping Recibido")
				else:
					#El random no era correcto, quita sesion
					ping_resp = 0
					notificacion_ap(int(uid),"Ping Fallido. Seras desconectado")
			contador = contador+1

		except ssl.SSLError,err:
			escribe_error(err)
	#ping timeout	
	if not ping_resp:
		#os.system("init 3")
		time.sleep(3)
		close_graphic_session(str(id_user))
		#os.system("init 5")
	else:
		cadena = struct.pack('>B1ih16s',3,0,int(id_user),'\x00'*16)
		client = ssl.wrap_socket(socket.socket(),
			ca_certs=str(directorio)+'servercert.pem',
			cert_reqs=ssl.CERT_REQUIRED,
			ssl_version=ssl.PROTOCOL_SSLv23)
		client.connect((server_admin, int(port_admin)))
		client.write(cadena)
		data = client.read()
		status, rand_encrypt = struct.unpack('>h16s',data)
		client.close()
		if status:
			#servidor autoriza que cierre la sesion el AP
			#os.system("init 3")
			time.sleep(3)
			close_graphic_session(str(id_user))
			#os.system("init 5")
	
def send_rand_encrypt(rand):
		time.sleep(0.4)
		command = struct.pack('>3s16s',"\xFF\x78\x13",rand)
		watch.write(command)
		watch.flushInput()
		time.sleep(0.15)
		#comprobar que se ha enviado correctamente
		
def start_auth():
		while True:
			#n = random.randint(1,2147483647)
			n = random.randint(1,999999999)
			#n = 16711424
			#print "S:	RANDOM:	" + str(n) + "			%s" % hex(n)
			command = struct.pack('>3s1i',"\xFF\x74\x07",n)
			watch.write(command)
			watch.flushInput()
			time.sleep(0.4)
			lectura_auth(n)

def lectura_auth(random):
		watch.write("\xFF\x75\x15\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
		time.sleep(0.5)
		data = watch.read(21)
		watch.flushOutput()
		cad,uid,rand_aes = struct.unpack('>3sh16s',data)
		if (uid != 0):
			cadena = struct.pack('>B1ih16s',1,random,uid,rand_aes)	
			try:
				client = ssl.wrap_socket(socket.socket(),
					ca_certs=str(directorio)+"servercert.pem",
					cert_reqs=ssl.CERT_REQUIRED,
					ssl_version=ssl.PROTOCOL_SSLv23)
				client.connect((server_admin, int(port_admin)))
				client.write(cadena)
				data = client.read()
				status, rand_encrypt = struct.unpack('>h16s',data)
				client.close()
				if status:
					send_rand_encrypt(rand_encrypt) # envia confirmacion
					open_graphic_session(str(uid)) # abre sesion
					#Deberia entrar en el bucle hasta que se desconecte el usuario
					ping_session(str(uid))
				#client.close()
			except ssl.SSLError,err:
				print "SSL ERROR: %s" % err

def stop_auth():
		print "Enviado STOP_AUTH"
		watch.write("\xFF\x76\x04\x00")

def send_packet():
		print "Envia paquete"
		n = random.randint(1,32751)
		command = struct.pack('>3s16s',"\xFF\x78\x13",'\x00'*16)
		watch.write(command)
		data = watch.read(19)
		print "%s" % repr(data[0:19])

def rec_payload():
		print "Read Data"
		while True:
			data = watch.read(9)
			#if (data[0] != ' '):
			print "%s" % repr(data)

def send_ack():
		time.sleep(0.4)
		command = struct.pack('>3s16s',"\xFF\x78\x13","\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
		watch.write(command)
		watch.flushInput()
		time.sleep(0.15)
	

def help_menu():
	print '''\
Help Menu for CC1111-Access Point
Options include:
 	--version		: Version del programa.
  	--certificate		: Recibe certificado del servidor de administracion
	--acc      		: Inicia el dispositivo para recoger los movimientos (Suprimir)
	--stop     		: Para el dispositivo. (Simpliciti,auth,serial port)
	--info     		: Muestra informacion del dispositivo
	--payload  		: Receive payload
	--stopauth 		: Auth Stop
	--help     		: Muestra el menu de ayuda	'''




if (len(sys.argv) == 1):
	pid = os.fork()
	if pid > 0:
		sys.exit(0)
	pid = os.fork()
	if pid > 0:
		sys.exit(0)
	if pid == 0:
		os.chdir("/")
		os.setsid()
		os.umask(0)
		sys.stdout.flush()
		sys.stderr.flush()
		si = file('/dev/null', 'r')
                so = file('/dev/null', 'a+')
                se = file('/dev/null', 'a+', 0)
                os.dup2(si.fileno(), sys.stdin.fileno())
                os.dup2(so.fileno(), sys.stdout.fileno())
                os.dup2(se.fileno(), sys.stderr.fileno())
		pid = str(os.getpid())
		file("/var/run/AP_CC1111.pid",'w+').write("%s\n" % pid)
		time.sleep(1)
		#wait to connect the device
		while not os.access('/dev/ttyACM0',os.R_OK):
			time.sleep(1)
		watch = eZ430.watch()
		time.sleep(1)
		start_auth()
if sys.argv[1].startswith('--'):
	option = sys.argv[1][2:]
	if option == 'version':
		print "Access-Point CC1111 %s" %__version__
	elif option == 'info': 
		#iniciamos el reloj
		watch = eZ430.watch()
		print "Informacion Device ",watch.dev
		lectura_info()
	elif option == "acc":
		#iniciamos el reloj
		watch = eZ430.watch()
		print "Opening eZ430 on ",watch.dev
		lectura_ACC()
	elif option == "start":
		print "Para iniciar el sistema use:\n service daemon_AP start\n 	[or]\n python AdminServer.py\n"
		help_menu()
	elif option == "payload":
		print "Opening eZ430 on ",watch.dev
		rec_payload()
	elif option == 'stop':
		print "Deteniendo CC1111"
		watch = eZ430.watch()
		watch.stop_simpliciti()
		watch.stop_auth()
		watch.close()
	elif option == 'stopauth':
		watch.stop_auth()
	elif option == 'help':
		help_menu()
	else:
		help_menu()
