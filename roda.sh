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
MAX_NUM_THREADS=8
K=1000

for NUM_THREADS in $(seq 0 $MAX_NUM_THREADS)
do

    TIME[$NUM_THREADS]=""
    MOPS[$NUM_THREADS]=""

    TIME_MED[$NUM_THREADS]=0
    MOPS_MED[$NUM_THREADS]=0

    for i in $(seq 1 $NUM_RODADAS)
    do
        OUTPUT=$( $KMENORES $NUM_ELEM $K $NUM_THREADS )

        TIME[$NUM_THREADS]="${TIME[$NUM_THREADS]}$(grep "Total_time" <(echo "$OUTPUT") | cut -d' ' -f2) "
        MOPS[$NUM_THREADS]="${MOPS[$NUM_THREADS]}$(grep "Throughput" <(echo "$OUTPUT") | cut -d' ' -f2) "

        TIME_MED[$NUM_THREADS]=$( echo ${TIME_MED[$NUM_THREADS]} + $(grep "Total_time" <(echo "$OUTPUT") | cut -d' ' -f2) | bc -l)
        MOPS_MED[$NUM_THREADS]=$( echo ${MOPS_MED[$NUM_THREADS]} + $(grep "Throughput" <(echo "$OUTPUT") | cut -d' ' -f2) | bc -l)
    done
    ##TIME_MED[$NUM_THREADS]=$( echo ${TIME_MED[$NUM_THREADS]}/$NUM_RODADAS | bc -l)
    ##MOPS_MED[$NUM_THREADS]=$( echo ${MOPS_MED[$NUM_THREADS]}/$NUM_RODADAS | bc -l)

    #echo "Threads:$NUM_THREADS"
    #echo "$TIME_MED"
    #echo "$MOPS_MED"

done

for i in $(seq 1 $NUM_RODADAS)
do
    for NUM_THREAD in $(seq 0 $MAX_NUM_THREADS)
    do
        #echo $NUM_THREADS
        echo -n "$(echo "${TIME[$NUM_THREAD]}" | cut -d' ' -f$i ) "
        #echo "${MOPS[$NUM_THREADS]}"
    done
    echo
done

for NUM_THREAD in $(seq 0 $MAX_NUM_THREADS)
do
    echo -n "$(echo "${TIME_MED[$NUM_THREAD]}*1000/$NUM_RODADAS" | bc -l)  "
done
echo

exit 0
