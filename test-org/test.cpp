


#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>


using namespace std;
typedef unsigned short ushort;
const char VALID_CHARS[] = "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ-0123456789 ~`!@#$%^&()+={}[];',";

inline unsigned short next(ushort num, ushort off, ushort len) {
    return (num + off) % len;
}

int buildMaps(const char* validChars, ushort off, ushort start, char* encMap, char* decMap) {
    ushort vLen = (ushort) strlen(validChars);
    ushort* hits  = new ushort[vLen];
    char* scrambled = new char[vLen+1];

    ushort num = next(start, off, vLen);
    ushort idx = 0; 
    ushort errCnt = 0; 
    ushort prev = start;

    do {
        if (hits[num]++ != 0) { 
            std::cout << "error ";
            errCnt++;
        }
        scrambled[idx++] = validChars[prev];
        prev = num;
        // std::cout << idx << ": " << num << std::endl;
        num = next(num, off, vLen);
    } while (num != start);
    scrambled[idx++] = validChars[prev];

    if (errCnt != 0 || idx != vLen) {
        std::cout << "Error mapping for key " << off << std::endl;
        for (ushort idx = 0; idx < vLen; idx++) {
            if (hits[idx] != 1) 
                std::cout << idx << ": " << hits[idx] << std::endl;
        }
    } else {
        std::cout << "No errors for key " << off << std::endl;
    }
    
    std::cout << " In=[" << validChars << "]\n";
    std::cout << "Out=[" << scrambled << "]\n";

    for (unsigned char c = 0x00; c != 0xff; c++) {
        const char* ptr = strchr(validChars, c);
        if (ptr != NULL) {
            unsigned int pos = ptr - validChars;
            unsigned char d = (unsigned char)scrambled[pos];
            encMap[c] = d;
            decMap[d] = c;
        } else {
            encMap[c] = c;
            decMap[c] = c;
        }
    }
    return errCnt;
}

void mapChar(const char* mapChar, const char* inStr, char* outStr) {
    unsigned inLen = strlen(inStr);
    for (unsigned idx = 0; idx < inLen; idx++) {
        char c = inStr[idx];
        outStr[idx] = mapChar[(unsigned)c];
    }
}

void encChar(const char* validChars, const char* mapChar, const char* inStr, char* outStr) {
    ushort vLen = (ushort) strlen(validChars);
    unsigned inLen = strlen(inStr);
    for (unsigned idx = 0; idx < inLen; idx++) {
        char c = inStr[idx];
        const char* pos = strchr(validChars, c);
        if (pos != NULL) {
            unsigned vIdx = (unsigned)(pos - validChars);
            c = validChars[(vIdx + idx) % vLen];
        }
        outStr[idx] = mapChar[(unsigned)c];
    }
}
void decChar(const char* validChars, const char* mapChar, const char* inStr, char* outStr) {
    ushort vLen = (ushort) strlen(validChars);
    unsigned inLen = strlen(inStr);
    for (unsigned idx = 0; idx < inLen; idx++) {
        char c = inStr[idx];
        c = mapChar[(unsigned)c];
        const char* pos = strchr(validChars, c);
        if (pos != NULL) {
            unsigned vIdx = (unsigned)(pos - validChars);
            c = validChars[(vIdx - idx) % vLen];
        }
        outStr[idx] = c;
    }
}

int main(int argc, char** argv) {
    ushort len = (ushort) strlen(VALID_CHARS);
    std::cout << "Valid chars=" << len << std::endl;

    ushort off = (argc > 1 && atoi(argv[1]) > 0) ? atoi(argv[1]) : 11;
    ushort start = (argc > 2 && atoi(argv[2]) >= 0) ? atoi(argv[2]) : 0;

    if ((len % off) == 0) {
        std::cerr << "Invalid key " << off << " must not be even divider of " << len << std::endl;
        return -1;
    }

    char encMap[256];
    char decMap[256];
    buildMaps(VALID_CHARS, off, start, encMap, decMap);

    const char* inStr = (argc > 3) ? argv[3] : "AAAaaa Hello World 12345";
    unsigned inLen = strlen(inStr);
    char* encStr = new char[inLen+1];
    char* decStr = new char[inLen+1];

#if 0
    mapChar(encMap, inStr, encStr);
    mapChar(decMap, encStr, decStr);
#else
    encChar(VALID_CHARS, encMap, inStr, encStr);
    decChar(VALID_CHARS, decMap, encStr, decStr);
#endif

    std::cout << " in=[" << inStr << "]\n";
    std::cout << "enc=[" << encStr << "]\n";
    std::cout << "dec=[" << decStr << "]\n";
    if (strcmp(inStr, decStr) == 0) 
        std::cout << "  Success\n";
    else
        std::cout << "  FAILED \n";

    return 0;
}
