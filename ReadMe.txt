exe2_his_2.4.0(kdb)
===============

Introduction
---------------

exe2_his (kdb) monitor kingbase databse 

Install
---------------
cp lib to ($D5000_HOME)/lib and start in root.

when the software write log if dir is not exist ,the software may core dump,
makesure ($D5000_HOME)/var/log is exist.

the software will invoke ./kdbDataMonitor/tabledspaceCollect.sh
and ./kdbDataMonitor/env.properties need to be config;

./conf/auto_monitor.conf need add virtualIp if databse switch need alarm;

