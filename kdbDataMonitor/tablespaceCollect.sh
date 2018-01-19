#!/bin/sh
########################################################################################################################################
###
### Descipt: the script is used to add datafiles to kingbase,before you run it,you should set the variables 
### 
########################################################################################################################################

GET_DBNAME="GET_ALL_DATABASE_NAME"

this_dir=`pwd`

dirname $0|grep "^/" >/dev/null
if [ $? -eq 0 ];then
    this_dir=`dirname $0`
else
    dirname $0 | grep "^\.$" >/dev/null
    if [ $? -ne 0 ];then
        this_dir=`dirname $0|sed "s#^#${this_dir}/#"`
    fi
fi
echo ""> ${this_dir}/result.txt
. ${this_dir}/env.properties
. ${this_dir}/function.sh
. ${this_dir}/env_adddatafile.properties
#####kingbase server test!#####
check_kb_service
KB_SERVICE=$?
#kingbase connection fails then exit script

############# Try to test weather need to add datafiles to kingbase,if need,do it ########
set_lib
var_block_size=`${KB_HOME}/bin/isql -h ${HOST} -p ${PORT} -U ${USER} -W ${PASSWORD} -c "show block_size" ${DATABASE} | tail -n 3 | awk '{if (NR == 1) print $1}'`
echo  block_size = "$var_block_size"

db_list=`${KB_HOME}/bin/isql -h ${HOST} -p ${PORT} -U ${USER} -W ${PASSWORD} -c "SELECT datname FROM sys_catalog.sys_database where datname not in ('TEMPLATE0','TEMPLATE1','TEMPLATE2','GLOBAL','TEMPDB',$NO_DATABASE) ORDER BY 1;" ${DATABASE} | awk -F \n 'NR>2 {print $0}' | sed -n -e :a -e '1,2!{P;N;D;};N;ba'`
db_list='HISDB'
echo ${db_list};

for db in `echo ${db_list} | sed 's/,/ /g'`; do
ts_list=`${KB_HOME}/bin/isql -h ${HOST} -p ${PORT} -U ${USER} -W ${PASSWORD} -c "SELECT spcname FROM sys_catalog.sys_tablespace where spcname not in ('TEMP') ORDER BY 1;" ${db} | awk -F \n 'NR>2 {print $0}' | sed -n -e :a -e '1,2!{P;N;D;};N;ba'`

   for ts in `echo ${ts_list} | sed 's/,/ /g'`; do 
     DATE_YMDHMS_NEW=`date '+%Y%m%d_%H%M%S'`
     var_size_tag=`${KB_HOME}/bin/isql -h ${HOST} -p ${PORT} -U ${USER} -W ${PASSWORD} -c "select sum(a.maxsize) - sum(a.usedblocks*${var_block_size}/1024/1024) as total_avilable from sys_datafiles a join sys_alldatafiles b on a.fileid=b.fileid where datname='${db}' and spcname='${ts}';" ${db} | tail -n 3 | awk '{if (NR == 1) print $1}'`
	echo ${db} ${ts} "$var_size_tag" >> result.txt
    done  
done


############## end ########
