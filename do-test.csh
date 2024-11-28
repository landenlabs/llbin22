#!/bin/tcsh

find DerivedData/Build/Products/Release -name llbin
set llbin=../DerivedData/Build/Products/Release/llbin

pushd test-run
    rm test*
    cp ../test-org/test* .

    $llbin -enc test* |& grep "^End"
    $llbin -dec test* |& grep "^End"

    # corrupt one file
    ls >! test.csh

    # compare files.
    echo --- Compare decrypt to original,  0 = files are the same.
    echo test.csh has been corrupt and should be different.
    foreach foo ( test* )
      cmp -z  $foo ../test-org/$foo
      echo "   $status $foo"
    end

    rm test*

    # The key option is not yet implemented. 
    if (0) then
        echo --- Test custom key and custom extension
        cp -n -f ../test-org/test.csh .
        llbin -key=hello -ext=.hello -enc test.csh |& grep "Checked"
        md5 test*
        llbin -key=hello -ext=.hello -dec test* |& grep "Checked"
        md5 test.csh ../test-org/test.csh
        rm test.*

        echo --- Test custom key and custom extension, decrypt with wrong key
        cp -n -f ../test-org/test.csh .
        llbin -key=world -ext=.world -enc test.csh |& grep "Checked"
        md5 test*
        llbin -key=hello -ext=.world -dec test* |& grep "Checked"
        md5 test.csh ../test-org/test.csh
        rm test.*
    endif
popd


