 #!/bin/bash
 src=`find src/ -name "*.h*"`
 mkdir inc
 echo $src
 for obj in $src
 do
    echo "cp header file $obj"
    cp --parents $obj inc/
 done
 zip -r inc.zip inc
