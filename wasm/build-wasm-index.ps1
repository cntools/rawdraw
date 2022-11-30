# Install LLVM to C:\Program Files\LLVM or set parameter below (Recommends LLVM-11)
# Install Binaryen to C:\binaryen\ or set parameter below (Recommends binaryen-98)
# If you want uglify-js, install node to your system paths, https://nodejs.org/en/download/
#  Execute: npm install -g terser

# You can also install clang via msys2 with "pacman -S llvm mingw-w64-clang-x86_64-wasm-pack mingw-w64-clang-x86_64-binaryen lld"

#$Clang = "C:\Program Files\LLVM\bin\clang"
$Clang = "D:\msys64\usr\bin\clang"

# Assuming you have downloaded binaryen-98 and put it someheere that is in your paths.
#$WASMOpt = "wasm-opt"
$WASMOpt = "D:\msys64\clang64\bin\wasm-opt"


$OutFile = 'index.html'
$TemplateHT = 'template.ht'
$TemplateJS = 'template.js'
$MainWASM = 'main.wasm'
$InputC = 'rawdraw.c'
$IntermediateJS = 'main.js.tmp'

Write-Host "Using Clang at $Clang"
Write-Host "Compiling $InputC..."
$ClangProc = Start-Process -NoNewWindow -FilePath $Clang -PassThru -ArgumentList @(
    '-I..',
    '-DWASM',
    '-nostdlib',
    '--target=wasm32',
    '-flto',
    '-Oz',
    '-Wl,--lto-O3',
    '-Wl,--no-entry',
    '-Wl,--allow-undefined',
    '-Wl,--import-memory',
    "-o $MainWASM",
    $InputC
)
$ClangProc.WaitForExit();

Write-Host "Using Binaryen at $WASMOpt"
Write-Host 'Asyncifying...'
$AsyncProc = Start-Process -NoNewWindow -FilePath $WASMOpt -PassThru -ArgumentList @(
    '--asyncify',
    '--pass-arg=asyncify-imports@bynsyncify.*',
    '--pass-arg=asyncify-ignore-indirect'
    '-Oz',
    $MainWASM,
    "-o $MainWASM.opt"
)
$AsyncProc.WaitForExit();

Write-Host 'Merging files...'
Write-Host ("WASM Binary size: {0:N0} B" -f (Get-Item $PSScriptRoot\$MainWASM).length)
$WASMasB64 = [Convert]::ToBase64String([IO.File]::ReadAllBytes("$PSScriptRoot\$MainWASM.opt"))
#Remove-Item $MainWASM

$ContentHT = Get-Content $TemplateHT -Raw
$ContentJS = Get-Content $TemplateJS -Raw

$Output = $ContentJS.Replace('${BLOB}', $WASMasB64);

if (Get-Command "terser" -ErrorAction SilentlyContinue) 
{
    Set-Content $IntermediateJS  -Value $Output
    Write-Host ("Before minifaction: {0:N0} B" -f ($Output).length)
    $Output = terser -ecma 2017 `
        -d RAWDRAW_USE_LOOP_FUNCTION=false `
        -d RAWDRAW_NEED_BLITTER=true `
        $IntermediateJS

    Write-Host ("After minifacation: {0:N0} B" -f ($Output).length)
	Remove-Item $IntermediateJS
}
else
{
    Write-Host 'terser not found. Not uglifying javascript.';
}

$Output = $ContentHT.Replace('${JAVASCRIPT_DATA}', $Output);

Set-Content $OutFile -Value $Output
Write-Host ("$OutFile size: {0:N0} B" -f (Get-Item $OutFile).length)
