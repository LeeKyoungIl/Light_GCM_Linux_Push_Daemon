Light_GCM_Linux_Push_Daemon
===========================

Linux Server Light Weight GCM (Google Cloud Messaging for Android) Push notification Daemon.


Requirements
-------------

* Linux Server (I recommend AWS t1.micro for testing.)
* MySQL Server instance (version 5.x later.)
* Linux gcc, curl library (Linux general) 

How to Install?
-------------

* MySQL Part

1) Install MySQL Server and MySQL C Client library
```bash
# yum install mysql-server mysql-devel
``` 
2) Run MySQL Server
```bash
# /etc/rc.d/init.d/mysqld start
``` 
3) Setting MySQL root info
```bash
# /usr/bin/mysql_secure_installation
``` 
4) The connection settings and create DB in MySQL console
```mysql
mysql> CREATE CREATE DATABASE push_center;
Query OK, 1 row affected (0.00 sec)

mysql> GRANT ALL PRIVILEGES ON push_center.* to 'push'@'127.0.0.1' IDENTIFIED BY 'gcm' WITH GRANT OPTION;
Query OK, 0 rows affected (0.00 sec)
```
5) Restore table using Schema file (Light_GCM_Linux_Push_Daemon/db_schema/table_schema.sql)
```bash
db_schema]# mysql -h 127.0.0.1 -u push -p push_center < table_schema.sql
```
6) Modify mysql_connect.c file (Light_GCM_Linux_Push_Daemon/mysql_connect.c)
```c
#include <mysql/mysql.h>

/**
 * MySQL connect info define
 *
 * - MYSQL_HOST : Insert MySQL server ip address or domain address
 * - MYSQL_PORT : Insert MySQL server port (ex 3306)
 * - MYSQL_USER : Insert MySQL user ID to use for server connection
 * - MYSQL_PASS : Insert MySQL user password to use for server connection
 * - MYSQL_DB : Insert MySQL DB name to use for push service
 */
#define MYSQL_HOST "127.0.0.1"
#define MYSQL_PORT 3306
#define MYSQL_USER "push"
#define MYSQL_PASS "gcm"
#define MYSQL_DB "push_center"

```

Ready Complete to Use Database.

* gcc, curl library install part

1) Install gcc and curl-devel 
```bash
# yum install gcc curl-devel
```

Ready to compile to Daemon end.

* Compile daemon

1) find libmysqlclient.so path
```bash
# find / -name libmysqlclient.so
/usr/lib64/mysql/libmysqlclient.so
```

2) compile Daemon
```bash
# gcc -o LLGCM_Push_Daemon LLGCM_Push_Daemon.c -lcurl -I/usr/include -L/usr/lib64/mysql/ -lmysqlclient -lm
```

```bash
drwxr-xr-x 2 root root  4096 Aug  4 05:42 db_schema
-rw-r--r-- 1 root root  5619 Aug  4 05:42 http_connect.c
-rwxr-xr-x 1 root root 23107 Aug  4 06:08 LLGCM_Push_Daemon
-rw-r--r-- 1 root root  4418 Aug  4 05:42 LLGCM_Push_Daemon.c
-rw-r--r-- 1 root root  2745 Aug  4 05:52 mysql_connect.c
-rw-r--r-- 1 root root  4932 Aug  4 05:42 push_util.c
-rw-r--r-- 1 root root   565 Aug  4 05:42 README.md
```

3) execute Daemon
```bash
# LLGCM_Push_Daemon
```

4) check Daemon
```bash
# ps aux | grep LLGCM
root      2417 18.7  0.2 161140  2088 ?        Ss   06:08   0:00 ./LLGCM_Push_Daemon
root     11428  0.0  0.0 110280   844 pts/0    S+   06:08   0:00 grep LLGCM_Push_Daemon
```

ChangeLog
-------------

ver 0.0.1 (2014/08/04)
* first commit

MIT License
-------------

Copyright (c) 2014 Lee Kyoung Il. See LICENSE.txt for details.
