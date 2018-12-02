qcc="qcc"
QuackOutput="QuackOutput"


rm -rf build

if [ -f $qcc ] ; then
    rm -rf $qcc
fi

if [ -f $QuackOutput ] ; then
    rm -rf $QuackOutput
    rm -rf QuackOutput.c
fi
