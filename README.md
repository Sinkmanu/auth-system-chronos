Proximity Authentication using eZ430-Chronos and CC1111.
============================================================

Authentication system by proximity using the eZ430-Chronos watch.

Involved elements
-----------------
```
Client:     eZ430-Chronos
AP:         CC1111 + PC	
Server:     PC	
```
	
Working
-------

We have made a protocol to communicate between the eZ430-Chronos and the CC1111 (and PC),  between these they communicate using symmetric cryptography with the AES algorithm as You can see in the figure 1. Between the Access Point and the Server, we use SSL. Figure 2. In the Figure 3, you can see a global vision of the system.

![ScreenShot](https://raw.github.com/wiki/Sinkmanu/auth-system-chronos/AP2Chronos.png)

<small>Figure 1.</small>

![ScreenShot](https://raw.github.com/wiki/Sinkmanu/auth-system-chronos/AP2Server.png)

<small>Figure 2.</small>

![ScreenShot](https://raw.github.com/wiki/Sinkmanu/auth-system-chronos/Generico.png)

<small>Figure 3.</small>

Installation
-----------

###For the client. 
You should read in the client ("Chronos Auth System" folder) README.

###For the AP. 
You should read the AP README.

###For the Server. 
You should read the Server README.


References and more details
---------------------------

####Slides
```
http://www.slideshare.net/ManuelMancera/presentacion-23634783
```



