#!/bin/tcsh

# find DerivedData/Build/Products/Release -name llbin22
set llbin22=../DerivedData/Build/Products/Release/llbin22

cd test-run
    rm test* >& /dev/null
    cp ../test-org/test* .

    echo --- Original files
    ls -al test*

    echo
    echo --- Encrypted files
    $llbin22 -enc test* |& grep "^End"
    ls -al test*
    
    $llbin22 -dec test* |& grep "^End"

    # corrupt one file
    ls >! test.csh

    # compare files.
    echo 
    echo --- Compare decrypt to original,  0 = files are the same.
    echo test.csh has been corrupt and should be different.
    foreach foo ( test* )
      cmp -z  $foo ../test-org/$foo
      echo "   $status $foo"
    end

    # rm test*

    # The key option is not yet implemented. 
    if (0) then
        echo --- Test custom key and custom extension
        cp -n -f ../test-org/test.csh .
        llbin22 -key=hello -ext=.hello -enc test.csh |& grep "Checked"
        md5 test*
        llbin22 -key=hello -ext=.hello -dec test* |& grep "Checked"
        md5 test.csh ../test-org/test.csh
        rm test.*

        echo --- Test custom key and custom extension, decrypt with wrong key
        cp -n -f ../test-org/test.csh .
        llbin22 -key=world -ext=.world -enc test.csh |& grep "Checked"
        md5 test*
        llbin22 -key=hello -ext=.world -dec test* |& grep "Checked"
        md5 test.csh ../test-org/test.csh
        rm test.*
    endif
cd ..


