llbin22
### Binary file encryption

### Builds
* OSX(M3)      | Provided Xcode project
* Windows/DOS  | Provided Visual Studio solution

### Visit home website
[https://landenlabs.com](https://landenlabs.com)

### Description

C++ v17 code to build either a windows/dos or Mac/Linux command line tool
that can scan directories and encrypt files.

***NOTE*** This is an experiment. 

### Help Banner:
<pre>
llbin22  Dennis Lang v2.4 (landenlabs.com) Dec 29 2024

Des: 'Manipulate Binary Files
Use: llbin22 [options] directories...   or  files

 Options (only first unique characters required, options can be repeated):

   -encrypt                   ; Encrypt and rename file foo.ext to foo-e.b22
   -decrypt                   ; Decrypt and rename file foo-e.b22 foo.ext

   -hideDir                   ; Hide (obfuscate) directory names
   -unhideDir                 ; Reverse obfuscated directory names
   -hideFile                  ; Hide (obfuscate) file names
   -unhideFile                ; Reverse obfuscated file names

   -includeFile=&lt;filePattern> ; Include files by regex match
   -excludeFile=&lt;filePattern> ; Exclude files by regex match
   -IncludePath=&lt;pathPattern> ; Include path by regex match
   -ExcludePath=&lt;pathPattern> ; Exclude path by regex match
   -verbose
   -norun

 Optional:
   -ext=&lt;extension>       ; Use with -encrypt to set file extension

 Example:
   llbin22 -inc=a*.png -inc=*.jpg -ex=foo.jpg -key=123456 -encrypt dir1/subdir dir2
   llbin22 -inc=a*.aax -inc=b*.aax -ex=foo.* -encrypt dir1/subdir dir2/subdir dir3/subdir
   llbin22 -inc=*b22 -decryp dir1/subdir dir2/subdir dir3/subdir


</pre>
