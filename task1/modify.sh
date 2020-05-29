#!/bin/sh

HELP='usage:\n\tmodify [-r] [-l|-u] <dir/file names...>\n\tmodify [-r] <sed pattern> <dir/file names...>\n\tmodify [-h | --help]\n'

function changeNames {
    filename=$(basename $1)
    filedir=$(dirname $1)

    if [ -z $sed_pattern ]
    then
	newname=${filename}
    else
	newname="$(echo ${filename} | sed ${sed_pattern})"
    fi
    
    if [ ${options[1]} ]
    then
	if test ${options[1]} = 'l';
	then
	    newname=${newname,,}
	    mv ${filedir}/${filename} ${filedir}/${newname}
	elif test ${options[1]} = 'u';
	then
	    newname=${newname^^}
	    mv ${filedir}/${filename} ${filedir}/${newname}
	fi
    else
	mv ${filedir}/${filename} ${filedir}/${newname}
    fi
}

function recursive {
    for f in ${path}/*;
    do
	if [ -d $f ]
	then
	    ${xname} -${options[0]} -${options[1]} $f $sed_pattern
	else
	    changeNames $f
	fi
    done
    
}

function checkHelp {
    for i in ${params[*]}
    do
	if [ $i == "-h" ] || [ $i == "--help" ]
	then
	    echo -en $HELP
	    exit
	fi
    done
}

function getParams {
    for i in ${params[*]}
    do
	case $i in
	    \?)
		echo "Invalid argument : -$OPTARG. Use --help to check usage." >&i
		exit
		;;
	    -r) options[0]='r'
		;;
	    -l) options[1]="l"
		;;
	    -u) options[1]="u"
		;;
	    *)
		if [ -f $i ] || [ -d $i ];
		then
		    path=$i
		elif [ -n $i ];
		then
		    sed_pattern=$i
		else
		    echo "INVALID PATH"
		    exit
		fi
		;;
	esac
    done
}

#-------------------------------------------------------------

params=($*);
options=();
xname="./modify.sh";

checkHelp;
getParams;

if [ options[0]=='r' ] && [ -d ${path} ]
then
    recursive;
else
    changeNames ${path}
fi
