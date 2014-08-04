Light_GCM_Linux_Push_Daemon
===========================

Linux Server Light Weight GCM (Google Cloud Messaging for Android) Push notification Daemon.


Requirements
-------------

* Linux Server (I recommend AWS t1.micro for testing.)
* MySQL Server instance (version 5.x later.)

How to Install?
-------------

1. Install MySQL Server and MySQL C Client library

```bash
    yum install mysql-server mysql-devel
``` 

2. Run MySQL Server

```bash
  /etc/rc.d/init.d/mysqld start
``` 

3. Setting MySQL root info

```bash
  /usr/bin/mysql_secure_installation
``` 

