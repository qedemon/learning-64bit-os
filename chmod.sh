#!/bin/bash
files=$(find . -not -path "./.*" )
for file in $files
do
    if [ "$file" -ef ./chmod.sh ]
    then
        :
    else
        chmod go-rwx $file
        if [ -d "$file" ]
        then
            chmod u+rwx $file
            chmod go+rx $file
        else
            chmod a+rw $file
            chmod a-x $file
            chmod go+r $file
        fi
#        echo $file
    fi
done