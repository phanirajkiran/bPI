#! /bin/bash
# This script is used to visualize continuous data output by the class
# CommandWatchValues via serial line
# feedgnuplot (https://github.com/dkogan/feedgnuplot) is used for visualization

cd "$(dirname $(readlink -f $0))"
script_name="$0"


# the binary
feedgnuplot=feedgnuplot

value_name=""
win_len=800
update_freq=1

dev="/dev/ttyUSB0"
[ ! -e $dev ] && dev="/dev/ttyUSB1"



usage() {
	echo "Usage: $script_name"
	echo "       [-d,--device <device>]  select input device (default=$dev)"
	echo "       [-v,--value <value>]    select value to plot (default=$value_name)"
	echo "                               if it is empty, it is assumed there is only one"
	echo "       [-u,--update-freq <value>]"
	echo "                               plotting update frequency: replot every x values (default=$update_freq)"
	echo "       [-l,--xlen <length>]    output x-axis window length (default=$win_len, 0=dont cut)"
	echo "       [-f,--feedgnuplot <bin>]"
	echo "                               select feedgnuplot binary (default=$feedgnuplot)"
	echo "       [-c,--cmd <cmd>]        send command before executing (eg to show values)"
	echo ""
	echo "Examples:"
	echo " High frequency update:"
	echo "  $script_name -c \"sensors noclear 1\" -l 4000 -v altitude -u 20"
	echo " additionally ensure to exit previous command:"
	echo "  echo q > /dev/ttyUSB*; $script_name -c \"sensors noclear 1\" -l 4000 -v altitude-filtered -u 20"
	echo " Low frequency update:"
	echo "  $script_name -c \"attitude noclear 100\" -l 800"
	exit -1
}

presend_cmd=""

while [ $# -gt 0 ]
do
	key="$1"
	shift
	case $key in
		-h|--help)
			usage
			;;
		-d|--device)
			dev="$1"
			shift
			;;
		-v|--value)
			value_name="$1"
			shift
			;;
		-u|--update-freq)
			update_freq="$1"
			shift
			;;
		-l|--xlen)
			win_len="$1"
			shift
			;;
		-f|--feedgnuplot)
			feedgnuplot="$1"
			shift
			;;
		-c|--cmd)
			presend_cmd="$1"
			shift
			;;
	esac
done

if [ ! -e $dev ]; then
	echo "Error: device $dev does not exist" >&2
	exit -1
fi


[ ! -z "$presend_cmd" ] && echo "$presend_cmd" > $dev

win_len_cmd=""
[ "$win_len" -gt 0 ] && win_len_cmd="--xlen $win_len"

filter_cmd=""
if [ ! -z "$value_name" ]; then
	filter_cmd="if(\$1==\"$value_name\") "
fi

cat $dev | awk -F ',* ' \
	"BEGIN{n=0}{ $filter_cmd"' {for(i=2;i<=NF;++i) printf "%f ",$i; printf ORS; ++n; if(n % '$update_freq'==0) { print "replot" fflush(); } }}' | \
	$feedgnuplot --lines $win_len_cmd --stream trigger --title "$value_name" 2>/dev/null

