#!/bin/bash

[ $# -ne 1 ] && { 
    echo "ERRO: Numero de parametros invalido.";
    echo "USO: $0 <num de elementos>";
    exit 1;
 }

KMENORES="./acharKMenores"

make clean &>/dev/null
make &>/dev/null

NUM_ELEM=$1
NUM_RODADAS=10

if [ $# -eq 2 ] 
then
    K=$2
else
    K=2048
fi

echo "num elementos: $NUM_ELEM"
echo "num rodadas: $NUM_RODADAS"
echo "k : $K"

for NUM_THREADS in 0 2 3 4 5 6 7 8
do

    TIME[$NUM_THREADS]=""
    MOPS[$NUM_THREADS]=""

    TIME_MED[$NUM_THREADS]=0
    MOPS_MED[$NUM_THREADS]=0

    for i in $(seq 1 $NUM_RODADAS)
    do
        echo $KMENORES $NUM_ELEM $K $NUM_THREADS
        OUTPUT=$( $KMENORES $NUM_ELEM $K $NUM_THREADS )

        echo 1
        TIME[$NUM_THREADS]="${TIME[$NUM_THREADS]}$(grep "Total_time" <(echo "$OUTPUT") 2>/dev/null | cut -d' ' -f2) "
        echo 2
        MOPS[$NUM_THREADS]="${MOPS[$NUM_THREADS]}$(grep "Throughput" <(echo "$OUTPUT") 2>/dev/null | cut -d' ' -f2) "
        echo 3

        #TIME_MED[$NUM_THREADS]=$( echo ${TIME_MED[$NUM_THREADS]} + $(grep "Total_time" <(echo "$OUTPUT") 2>/dev/null | cut -d' ' -f2) | bc -l )
        #MOPS_MED[$NUM_THREADS]=$( echo ${MOPS_MED[$NUM_THREADS]} + $(grep "Throughput" <(echo "$OUTPUT") 2>/dev/null | cut -d' ' -f2) | bc -l )
    done
    ##TIME_MED[$NUM_THREADS]=$( echo ${TIME_MED[$NUM_THREADS]}/$NUM_RODADAS | bc -l)
    ##MOPS_MED[$NUM_THREADS]=$( echo ${MOPS_MED[$NUM_THREADS]}/$NUM_RODADAS | bc -l)
    #echo "Threads:$NUM_THREADS"
    #echo "$TIME_MED"
    #echo "$MOPS_MED"
done

for i in $(seq 1 $NUM_RODADAS)
do
    for NUM_THREAD in 0 2 3 4 5 6 7 8
    do
        #echo $NUM_THREADS
        echo -n "$(echo "${TIME[$NUM_THREAD]}" | cut -d' ' -f$i ) "
        #echo "${MOPS[$NUM_THREADS]}"
    done
    echo
done

#echo "Tempo Medio (s):"
#for NUM_THREAD in 0 2 3 4 5 6 7 8
#do
#    echo -n "$(echo "${TIME_MED[$NUM_THREAD]}*1000/$NUM_RODADAS" | bc -l)  "
#done
#
#echo "Vazao Media (MOP/s):"
#for NUM_THREAD in 0 2 3 4 5 6 7 8
#do
#    echo -n "$(echo "${MOPS_MED[$NUM_THREAD]}*1000/$NUM_RODADAS" | bc -l)  "
#done
#echo

exit 0
