<table border="0">
  <tr>
    <td>
      <!-- VERSION -->v6.05.25<br>
      24-May-2026<br>
      macOS<br>
      <a href="https://landenlabs.com">Home</a>
    </td>
    <td>
      <a href="https://landenlabs.com">
        <img src="screens/landen_labs_300.webp" width="300" alt="LanDen Labs">
      </a>
    </td>
  </tr>
</table>

# llbin22
### Binary file encryption

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE.txt)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Windows-lightgrey.svg)

### Description

C++17 command line tool (builds on Windows/DOS or Mac/Linux) that scans
directories and encrypts/decrypts files, and can obfuscate (hide) file
and directory names.

***NOTE*** This is an experiment.

### Builds
* OSX(M3)      | Provided Xcode project
* Windows/DOS  | Provided Visual Studio solution

### Dependencies
* [llcommon](https://github.com/landenlabs/llcommon) - shared LanDen Labs utility library (git submodule)

### Visit home website
[https://landenlabs.com](https://landenlabs.com)

### Help Banner:
<pre>
llbin22  Dennis Lang  <!-- VERSION -->v6.05.25

Des: Manipulate Binary Files
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

### License

```
Copyright 2026 Dennis Lang

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
See [LICENSE.txt](LICENSE.txt) for the full license text.

[Top](#top)