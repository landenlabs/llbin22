@echo off

set llbin22=llbin22

if exist test (
  echo Removing old test directory
  echo.
  rmdir /s test 
)
mkdir test
if not exist test (
  echo ERROR - unable to create test directory
  goto end
)
xcopy /s test-org test
 
cd test
    echo.
    echo === Encrypt  %llbin22% -enc test* 
    %llbin22% -enc test* 
    
    echo.
    echo === Decrypt %llbin22% -dec test* 
    %llbin22% -dec test* 

    :: corrupt one file to verify the cmp is working
    dir > test.csh

    :: compare files.
    echo.
    echo === Compare decrypt to original,  0 = files are the same.
    echo test.csh has been modified and should be different.
    for %%f in (test*) do ( 
      cmp -v %%f ../test-org/%%f
      :: echo  %ERRORLEVEL% %%f
      echo.
    )
cd ..

:end
echo [done]

