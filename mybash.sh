#!/bin/bash

#SHMANTIKO NA KLHTHEI AUTO TO BASH SCRIPT ME arguments -e filename GIA NA PAREI TO LOG POY THELOYME 


#akoloythei mia methodos gia to pws pairnoyme to pio frequent element se ena array:
#https://stackoverflow.com/questions/43440425/most-frequent-element-in-an-array-bash-3-2

get_result(){
awk '
  { 
      n=++hsh[$1]
      if(n>max_occ){
         max_occ=n
         what=$1
      }else if(n==max_occ){
         if(what>$1) 
             what=$1
      }
  } 
  END { print what }
'
}

#akoloythei mia methodos gia na parw to logfile poy thelw na eksetasw mesw command line kata thn klhsh tou bash autou. Sto parakatw link:
#https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash
POSITIONAL=()
lnkeeper=()
ARRAY=()
ARR2=()

while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -e|--extension)
    EXTENSION="$2"
    shift # past argument
    shift # past value
    ;;
    --default)
    DEFAULT=YES
    shift # past argument
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

echo FILE READ  = "${EXTENSION}"


i=0
while IFS='' read -r line || [[ -n "$line" ]]; do
    lnkeeper[i]=$line
	i+=1
done < "${EXTENSION}"




for i in "${lnkeeper[@]}"; do
	IFS=':' read -r -a ARRAY <<< "$i"
	ARR2+=(${ARRAY[2]})
done


tLen=${#ARR2[@]}
printf "%s" "Total number of keywords searched: " $tLen
echo ""


result=$(printf "%s\n" "${ARR2[@]}" |  get_result)
printf "%s" "Keyword most frequently found: "
echo $result

