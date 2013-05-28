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
Interfaz de Administracion para el servidor de los APs.
"""
import serial
import binascii
import time
import sys
import string
import datetime
import struct
import os
import socket
import ssl
from Crypto.Cipher import AES
from ConfigParser import SafeConfigParser
from optparse import OptionParser

import dbus, dbus.proxies


__author__="Manuel Mancera (sinkmanu@gmail.com)"
__date__ ="$04.12.2012 19:43:21$"
__version__ = "1.0"


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

#Iniciacion de variables desde el archivo de configuracion
parser = SafeConfigParser()
parser.read('/etc/adminssl.conf') #deberia estar en /etc/adminssl.conf
directorio=parser.get('configure_files','path_administrator') #path_administrator sera /usr/share/...

AUTH_ESTADO=1
SESSION_ESTADO=2
FIN_SESSION=3

def esta_user(username):
	fich = open(parser.get('configure_files','data_file'),"r")
	cont = 0
	e = 0
	lines = len(fich.readlines())
	fich.seek(0)
	while cont < lines and e==0:
		cad = fich.readline()
		if username == cad.split(":")[1]:
			e = 1
		else:
			cont = cont+1
	fich.close()
	if e == 1:
		return 1
	else:
		return 0

def esta_id(id_user):
	fich = open(parser.get('configure_files','data_file'),"r")
	cont = 0
	e = 0
	lines = len(fich.readlines())
	fich.seek(0)
	while cont < lines and e==0:
		cad = fich.readline()
		if id_user == cad.split(":")[0]:
			e = 1
		else:
			cont = cont+1
	fich.close()
	if e == 1:
		return 1
	else:
		return 0

def get_pass(id_user):
	fich = open(parser.get('configure_files','data_file'),"r")
	cont = 0
	e = 0
	lines = len(fich.readlines())
	fich.seek(0)
	while cont < lines and e==0:
		cad = fich.readline()
		if id_user == cad.split(":")[0]:
			e = 1
			password = cad.split(":")[2]
		else:
			cont = cont+1
	fich.close()
	if e == 1:
		return password
	else:
		return 0

def get_user(id_user):
	fich = open(parser.get('configure_files','data_file'),"r")
	cont = 0
	e = 0
	lines = len(fich.readlines())
	fich.seek(0)
	while cont < lines and e==0:
		cad = fich.readline()
		if id_user == cad.split(":")[0]:
			e = 1
			name = cad.split(":")[1]
		else:
			cont = cont+1
	fich.close()
	if e == 1:
		return name
	else:
		return 0

def bad_random(random,uid):
	if os.access(parser.get('logs','path_random')+str(uid),os.R_OK):
		fich = open(parser.get('logs','path_random')+str(uid),"r")
		cont = 0
		e = 0
		lines = len(fich.readlines())
		fich.seek(0)
		while cont < lines and e==0:
			cad = fich.readline()
			if random == cad.replace("\n", ''):
				e = 1
			else:
				cont = cont+1
		fich.close()
		return e
	else:
		return 0

def reset_permissions():
	fich = open(parser.get('logs','log_error'),"a")
	fich.close()
	fich = open(parser.get('logs','log_file'),"a")
	fich.close()
	os.chmod(parser.get('logs','log_error'), 0600)
	os.chmod(parser.get('logs','log_file'), 0600)
	os.chmod(parser.get('configure_files','data_file'),0600)
		

def log_error(error):
	fich = open(parser.get('logs','log_error'),"a")
	fich.write(unicode(datetime.datetime.now())+": ERROR: "+str(error)+"\n")
	fich.close()
		
def log_random(uid,random):
	fich = open(parser.get('logs','path_random')+str(uid),"a")
	fich.write(random+"\n")
	fich.close()
	os.chmod(parser.get('logs','path_random')+str(uid), 0600)

def log_server(uid,status,rand,source):
	fich = open(parser.get('logs','log_file'),"a")
	fich.write(unicode(datetime.datetime.now())+": "+ str(socket.gethostname())+": "+ status +" "+get_user(str(uid))+"(uid="+str(uid)+")	random="+str(rand)+"	src="+str(source)+"\n")
	fich.close()

def log_server_random(uid,status,rand_r,rand_s,source):
	fich = open(parser.get('logs','log_file'),"a")
	fich.write(unicode(datetime.datetime.now())+": "+ str(socket.gethostname())+": "+ status +" "+get_user(str(uid))+"(uid="+str(uid)+")	random_r="+str(rand_r)+"	random_s="+str(rand_s)+"	src="+str(source)+"\n")
	fich.close()

def log_server_fail(uid,status,source):
	fich = open(parser.get('logs','log_file'),"a")
	fich.write(unicode(datetime.datetime.now())+": "+ str(socket.gethostname())+": "+ status +"	uid="+str(uid)+"	src="+str(source)+"\n")
	fich.close()

def log_server_fail_certificate(source):
	fich = open(parser.get('logs','log_file'),"a")
	fich.write(unicode(datetime.datetime.now())+": "+ str(socket.gethostname())+": Certificate error verification	:	src="+ str(source) +"\n")
	fich.close()

def log_server_fail_lenght(source):
	fich = open(parser.get('logs','log_file'),"a")
	fich.write(unicode(datetime.datetime.now())+": "+ str(socket.gethostname())+": Bad lenght received	:	src="+ str(source) +"\n")
	fich.close()

def intro_datos(username,password,id_user):
	fich = open(parser.get('configure_files','data_file'), "a")
	if esta_user(username) == 0 and esta_id(id_user) == 0:
		time = unicode(datetime.datetime.now())
		cad = unicode(id_user)+":"+username +":"#+password+":"+time+"\n"
		fich.write(cad)
		fich.write(password.decode('hex'))
		cad = ":"+time+"\n"
		fich.write(cad)
		print "Added:	"+ username + "	ID:	" + str(id_user) +"	KEY:	"+ repr(password)
	else: 
		print "El usuario/id ya esta en el servidor."
	fich.close()

def quita_user(username):
	
	if esta_user(username) == 1:
		fich = open(parser.get('configure_files','data_file'), "r")
		cont = 0
		e = 0
		lines = len(fich.readlines())
		fich.seek(0)
		while cont < lines and e==0:
			cad = fich.readline()
			if username == cad.split(":")[1]:
				e = 1
			else:
			 	cont=cont+1
		fich.seek(0)
		fout = open(parser.get('configure_files','data_file')+".old","w")
		line = 0
		while line < lines:
			cad = fich.readline()
			if (line != cont):
				fout.write(cad)
			line=line+1
		fich.close()
		os.remove(parser.get('configure_files','data_file'))
		os.rename(parser.get('configure_files','data_file')+".old",parser.get('configure_files','data_file'))
		fout.close()
		print "Eliminado : "+ username 
	else:
		print "El usuario no esta en el servidor."

def busca_user(username):
	fich = open(parser.get('configure_files','data_file'), "r")
	cont = 0
	e = 0
	lines = len(fich.readlines())
	fich.seek(0)
	while cont < lines and e==0:
		cad = fich.readline()
		if username == cad.split(":")[1]:
			print cad.split(":")[0] +":" + cad.split(":")[1]+":" + get_pass(cad.split(":")[0]).encode('hex')+":" + cad.split(":")[3]+cad.split(":")[4]+cad.split(":")[5].strip('\n')
			e = 1
		cont=cont+1
	if e == 0:
		print "El usuario no esta registrado.\n"
	fich.close()

def start():
	try:
		server = ssl.wrap_socket(socket.socket(), server_side=True, keyfile=parser.get('server','private_key'),certfile=parser.get('server','cert'),
			ssl_version=ssl.PROTOCOL_SSLv23)
		server.bind((parser.get('server','address'), int(parser.get('server','port'))))
	except socket.error,err:
		log_error(err)
		raise SystemExit
	except ssl.SSLError,err:
		log_error(err)
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
		file("/var/run/adminSSL.pid",'w+').write("%s\n" % pid)
		while True:
			try:
				server.listen(5)
				socket_cliente, (host_n,port_n) = server.accept()
				(addr_client,port_client) = socket_cliente.getsockname()
				datos = socket_cliente.read()
				estado,random,uid,aes_random = struct.unpack('>B1ih16s',datos) ##datos = random,id,AES(random)
				if estado==AUTH_ESTADO:
					if esta_id(str(uid)):
						pass_watch = get_pass(str(uid))
						obj = AES.new(pass_watch,AES.MODE_ECB)
						rand_padd = obj.decrypt(aes_random)
						rand,relleno = struct.unpack('>1i12s',rand_padd)
						if (random|int(0xFF0000)) == (rand|int(0xFF0000)):
							if not bad_random(str(random),str(uid)):
						        	rand_en = struct.pack('>1i12s',random+1,'\x00'*12)
								rand_encrypt = obj.encrypt(rand_en)
								socket_cliente.write(struct.pack('>h16s',1,rand_encrypt))
								log_random(str(uid),str(rand))
								log_server(str(uid),"Session opened by",str(rand),str(addr_client))
								
							else:

								log_server(str(uid),"Random repeat by",str(rand),str(addr_client))						
						else:
							socket_cliente.write(struct.pack('>h16s',0,'\x00'*16))
							log_server_random(str(uid),"Authentication failure by",str(rand),str(random),str(addr_client))
							#log_server(str(uid),"Authentication failure by",str(rand),str(addr_client))
					else:	
						if uid != 0:
							socket_cliente.write(struct.pack('>h16s',0,'\x00'*16))
							log_server_fail(str(uid),"Authentication failure by unknown",str(addr_client))
					socket_cliente.close()
				elif estado==SESSION_ESTADO:
					if esta_id(str(uid)):
						pass_watch = get_pass(str(uid))
						obj = AES.new(pass_watch,AES.MODE_ECB)
						rand_padd = obj.decrypt(aes_random)
						rand,relleno = struct.unpack('>1i12s',rand_padd)
						if (random|int(0xFF0000)) == (rand|int(0xFF0000)):
							socket_cliente.write(struct.pack('>h16s',1,'\x00'*16))
						else:
							socket_cliente.write(struct.pack('>h16s',0,'\x00'*16))
							log_server_random(str(uid),"Ping Fail. Bad random",str(rand),str(random),str(addr_client))
					socket_cliente.close()
				elif estado==FIN_SESSION:
					if esta_id(str(uid)):
						socket_cliente.write(struct.pack('>h16s',1,'\x00'*16))
						log_server(str(uid),"Ping Timeout by",str(0),str(addr_client))
					socket_cliente.close()
			except ssl.SSLError,err:
				log_error(err)
				#log_server_fail_certificate(host_n) 
			except ssl.SSLEOFError,err:
				log_error(err)
			except socket.error,err:
				log_error(err)
			except struct.error,err:
				log_server_fail_lenght(str(addr_client))
		server.close()

def help_menu():
		print '''\
Help Menu 
Options include:
   -v , --version  			 Version del programa.
   -a ,	--add <user> <password> <id> 	 Introduce datos de usuarios en el servidor.  
   -d , --del <user>     		 Elimina usuario del servidor.
   -s , --search <user>    		 Busca usuario en el servidor.
   -S , --start				 Inicia servidor 
   -p , --permissions			 Resetea permisos de los archivos
   -g , --get_pass <id>     		 Muestra clave actual del uid
   -h . --help     			 Muestra el menu de ayuda	'''


def opciones():
	parser = OptionParser("usage: %prog [options] [arg1] [arg2] [arg3]")
	parser.add_option("-v", "--version",
                  action="store_true", dest="version", help="Version del programa")
	parser.add_option("-S", "--start",
                  action="store_true", dest="start", help="Inicia el servicio")
	parser.add_option("-P", "--permissions",
                  action="store_true", dest="permissions", help="Restablece permisos de los archivos")
	parser.add_option("-A", "--add",
                  action="store_true", dest="add", help="Introduce datos de usuario en el servidor, necesita nombre, uid y password")
	parser.add_option("-D", "--del",
                  action="store_true", dest="delete", help="Elimina usuario del servidor, necesita nombre")
	parser.add_option("-n", "--name",
                  action="store", type="string", dest="name", help="Nombre del usuario")
	parser.add_option("-u", "--uid",
                  action="store", type="string", dest="uid", help="UID del usuario")
	parser.add_option("-p", "--password",
                  action="store", type="string", dest="password", help="Password del usuario")
	parser.add_option("-s", "--search",
                  action="store_true", dest="search", help="Busca usuario en el servidor, requiere parametro nombre")
	parser.add_option("-g", "--get password",
                  action="store_true", dest="get", help="Muestra clave actual del uid, necesita parametro uid")
	
	
	

	(options, args) = parser.parse_args()


	if (len(sys.argv) == 1):
		parser.print_help()
	elif (options.version):
		print "Administrator-Server for Auth System version %s" %__version__
	elif (options.start):
		start()
	elif (options.permissions):
		reset_permissions()
		print "Permisos reseteados."
	elif (options.add):
		if (str(options.name) != "None") and (str(options.uid) != "None") and (str(options.password) != "None"):
			intro_datos(options.name,options.password,options.uid)
			reset_permissions()
		else: 
			print "Faltan argumentos: use ./admineZ430.py -A -n <user> -u <uid> -p <password>"
	elif (options.search):
		if (str(options.name) != "None"):
			busca_user(options.name)
		else: 
			print "Faltan argumentos: use ./admineZ430.py -s -n <user>"
	elif (options.delete):
		if (str(options.name) != "None"):
			quita_user(options.name)
			reset_permissions()
		else:
			print "Faltan argumentos: use ./admineZ430.py -D -n <user>"
	elif (options.get):
		if (str(options.uid) != "None"):
			if (get_pass(options.uid) != 0):
				print get_user(options.uid) +":"+ get_pass(options.uid).encode('hex')
			else: 
				print "UID no encontrado."
		else:
			print "Faltan argumentos: use ./admineZ430.py -g -u <uid>"


if __name__=="__main__":
	if (os.getuid() != 0):
		print "Deberias ejecutar ./admineZ430.py como root!"
		sys.exit(0)

	opciones()


