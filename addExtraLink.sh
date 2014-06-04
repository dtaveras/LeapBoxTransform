TARGET_DIR="./build/CMakeFiles/vtkTransformWidget.dir/"
if [ -d $TARGET_DIR ]
then
    echo $TARGET_DIR Exists => concatnating Extra Flags"
    echo " "${PWD}"/Leap/Lib/libc++/libLeap.dylib" > linkExtra.txt

    linkDir="./build/CMakeFiles/

    #Print links + Extra Links
    #echo $TARGET"link.txt" "linkExtra.txt" ">>" $TARGET_DIR"link.txt"
    perl -i -pe 'chomp if eof' $TARGET_DIR"link.txt"

    #Save link to link1
    cat $TARGET_DIR"link.txt" "linkExtra.txt" >> $TARGET_DIR"link1.txt"

    #Copy link1 to link
    cp $TARGET_DIR"link1.txt" $TARGET_DIR"link.txt"

    #remove link1
    rm $TARGET_DIR"link1.txt"
else
    echo "Make sure to name build directory => 'build'"
fi