@echo off

@echo Copy Release to d:\opt\bin
copy llbin-ms\x64\Release\llbin.exe d:\opt\bin\llbin.exe


@echo
@echo Compare md5 hash
cmp -h llbin-ms\x64\Release\llbin.exe d:\opt\bin\llbin.exe
ld -a d:\opt\bin\llbin.exe

@echo
@echo List all llbin.exe
ld -r -F=llbin.exe bin d:\opt\bin
