#!bin/sh

SERVICE="CHECK_KINGBASE_SERVICE"
SERVICE_ERROR_CODE=255
_DAILY="daily_"
COMMAND_RM="/bin/rm -rf"

#Check kingbase Service
check_kb_service()
{
    set_lib
	${KB_HOME}/bin/isql -h $HOST -p $PORT -U $USER -W $PASSWORD -c "select now();" $DATABASE > /dev/null 2>&1
}



#if dir not exsit,then create the dir.
mk_dir()
{
    _DIR=$1
    if [ ! -d ${_DIR} ] ;then
	      mkdir -p ${_DIR}
	      chmod 755 -R ${_DIR}
		echo "Folder ${_DIR} created." >> "${SCRIPT_LOG_PATH}"/${_DAILY}${DATE_YMD}.log
    fi
}

set_lib()
{
	 if [ `echo $LD_LIBRARY_PATH`'' == ""  ];then
      LD_LIBRARY_PATH=${LIBPATH}	
	 else
	   LD_LIBRARY_PATH=${LIBPATH}:${LD_LIBRARY_PATH}
	 fi
	 export LD_LIBRARY_PATH
}

#decide kingbase version
#6.1.3 return 0
#7.1.2 return 1
is_kb_version()
{
    set_lib
	  local VER=`${KB_HOME}/bin/kingbase -V`
	  local VER1=`echo ${VER} | awk '{print substr($0,21,5)}'`
	  if [ x"${VER1}" = x"6.1.3" ]; then
        return 0
	  elif [ x"${VER1}" = x"7.1.2" ]; then
        return 1
    fi
}

#split "," return the array
comma_split()
{
    local _split_name=$1
    _array=($(echo ${_split_name} | tr ',' ' ' | tr -s ' '))
}
