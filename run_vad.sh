#!/bin/bash

# Be sure that this file has execution permissions:
# Use the nautilus explorer or chmod +x run_vad.sh



DB=/mnt/c/Users/Ferran/Desktop/PAV/P2/db.v4
CMD=bin/vad  #write here the name and path of your program


for a1 in 8.15; do
    for a2 in 10.75; do
        for mst in 155; do
            for mvt in 20; do
                for iframes in 10; do
                    for filewav in $DB/*/*wav; do
                    #    echo
                        #echo "**************** $filewav ****************"
                        if [[ ! -f $filewav ]]; then 
                            echo "Wav file not found: $filewav" >&2
                            exit 1
                        fi

                        filevad=${filewav/.wav/.vad}

                        $CMD -i $filewav -o $filevad --a1 $a1 --a2 $a2 --mst $mst --mvt $mvt --initial $iframes|| exit 1

                    # Alternatively, uncomment to create output wave files
                    #    filewavOut=${filewav/.wav/.vad.wav}
                    #    $CMD $filewav $filevad $filewavOut || exit 1

                    done
                    echo "Values: alfa1: $a1, alfa2: $a2, mst: $mst, mvt: $mvt, iframes: $iframes"
                    scripts/vad_evaluation.pl $DB/*/*lab

                done
            done
        done
    done
done

exit 0
