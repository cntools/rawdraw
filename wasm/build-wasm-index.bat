
rem Install LLVM to C:\Program Files\LLVM or set parameter below (Recommends LLVM-11)
rem Install Binaryen to C:\binaryen\ or set parameter below (Recommends binaryen-98)

set CLANG="C:\Program Files\LLVM\bin\clang"
set WASMOPT="C:\binaryen\binaryen-version_98\bin\wasm-opt"

echo off
set OUTFILE=index.html
set TEMPLATEHT=template.ht
set TEMPLATEJS=template.js
set MAINWASM=main.wasm
echo on

%CLANG% -I.. -DWASM -nostdlib --target=wasm32 -flto -Oz -Wl,--lto-O3 -Wl,--no-entry -Wl,--allow-undefined -Wl,--import-memory -o main.wasm rawdraw.c
%WASMOPT% --asyncify --pass-arg=asyncify-imports@bynsyncify.* --pass-arg=asyncify-ignore-indirect -Oz main.wasm -o main.wasm

echo off
echo #include "stdio.h" > process.c
echo #include "stdlib.h" >> process.c
echo #include "string.h" >> process.c
echo #include "stdint.h" >> process.c
echo #define FILETOSTR( fname, str, len, mode ) { FILE * f = fopen( fname, mode ); fseek( f, 0, SEEK_END ); len = ftell( f ); fseek( f, 0, SEEK_SET ); str = malloc(len+1); fread( str, 1, len, f ); str[len] = 0; fclose(f); }   >> process.c
echo char * base64( uint8_t * data, int input_length ) { >> process.c
echo const char * encoding_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; >> process.c
echo int output_length = 4 * ((input_length + 2) / 3); >> process.c
echo char *encoded_data = malloc(output_length+1); >> process.c
echo for (int i = 0, j = 0; i != input_length;) { >> process.c
echo uint32_t octet_a = i != input_length ? (unsigned char)data[i++] : 0; >> process.c
echo uint32_t octet_b = i != input_length ? (unsigned char)data[i++] : 0; >> process.c
echo uint32_t octet_c = i != input_length ? (unsigned char)data[i++] : 0; >> process.c
echo uint32_t triple = (octet_a * 65536 ) + (octet_b * 256) + octet_c; >> process.c
echo encoded_data[j++] = encoding_table[(triple / 262144) %% 0x40]; >> process.c
echo encoded_data[j++] = encoding_table[(triple / 4096 ) %% 0x40]; >> process.c
echo encoded_data[j++] = encoding_table[(triple / 64) %% 0x40]; >> process.c
echo encoded_data[j++] = encoding_table[(triple / 1) %% 0x40]; >> process.c
echo } >> process.c
echo const int mod_table[] = { 0, 2, 1 }; >> process.c
echo for (int i = 0; i != mod_table[input_length %% 3]; i++) >> process.c
echo encoded_data[output_length - 1 - i] = '='; >> process.c
echo encoded_data[output_length] = 0; >> process.c
echo return encoded_data; >> process.c
echo } >> process.c
echo int main() { >> process.c
echo   FILE * fOUT = fopen( "%OUTFILE%", "w" ); >> process.c
echo   int tmpHTLen; char * tmpHTStr; FILETOSTR( "%TEMPLATEHT%", tmpHTStr, tmpHTLen, "r" ); >> process.c
echo   int tmpJSLen; char * tmpJSStr; FILETOSTR( "%TEMPLATEJS%", tmpJSStr, tmpJSLen, "r" ); >> process.c
echo   int mainWLen; uint8_t * mainWStr; FILETOSTR( "%MAINWASM%", mainWStr, mainWLen, "rb" ); >> process.c
echo   char * HTPost = strstr( tmpHTStr, "${JAVASCRIPT_DATA}" ); >> process.c
echo   HTPost[0] = 0; HTPost += strlen( "${JAVASCRIPT_DATA}" ); >> process.c
echo   char * JSPost = strstr( tmpJSStr, "${BLOB}" ); >> process.c
echo   JSPost[0] = 0; JSPost += strlen( "${BLOB}" ); >> process.c
echo   fprintf( fOUT, "%%s", tmpHTStr ); fprintf( fOUT, "%%s", tmpJSStr ); fprintf( fOUT,"%%s", base64( mainWStr, mainWLen ) ); fprintf( fOUT, "%%s\n", JSPost ); fprintf( fOUT, "%%s\n", HTPost ); >> process.c
echo } >> process.c
echo on
%CLANG% -D_CRT_SECURE_NO_WARNINGS process.c -o process.exe
process