@echo off
chcp 65001 >nul
REM ═══════════════════════════════════════════════════════
REM  STC32G12K128 编译脚本 (Keil C251)
REM  
REM  使用方法:
REM    1. 修改下方 C251_PATH 为你的 Keil C251 安装路径
REM    2. 在 VSCode 中按 Ctrl+Shift+B 即可编译
REM  或者直接在命令行运行: build_stc32g.bat
REM ═══════════════════════════════════════════════════════

REM ────── 配置区 (请根据实际安装路径修改) ──────
set C251_PATH=C:\Keil_v5\C251
set STARTUP_FILE=%C251_PATH%\LIB\START251.A51
REM ──────────────────────────────────────────────

echo [编译] STC32G12K128 温湿度检测项目
echo ================================================

REM 设置 PATH 以便找到 C251.EXE / L251.EXE / OH251.EXE
set PATH=%C251_PATH%\BIN;%PATH%

REM 中间文件目录
set OUTDIR=obj
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

REM 源文件列表 (手动维护，新增 .c 文件时在此添加)
set SRC_FILES=main.c Public\Delay.c Drivers\I2C.c

REM ────── 步骤 1: 编译 .c → .obj ──────
set OBJ_LIST=
for %%f in (%SRC_FILES%) do (
    set "src=%%f"
    set "obj=%OUTDIR%\%%~nf.OBJ"
    set OBJ_LIST=!OBJ_LIST! !obj!
    
    echo   C251: %%f
    C251.EXE "%%f" DB OE OPTIMIZE(9,SPEED) WARNINGLEVEL(2) OBJECT("!obj!") INCDIR(Public;Drivers;) 2>&1
    if errorlevel 1 (
        echo [错误] 编译失败: %%f
        exit /b 1
    )
)

REM ────── 步骤 2: 编译启动文件 ──────
if exist "%STARTUP_FILE%" (
    echo   A251: START251.A51
    A251.EXE "%STARTUP_FILE%" OBJECT("%OUTDIR%\START251.OBJ") 2>&1
    set OBJ_LIST=%OUTDIR%\START251.OBJ !OBJ_LIST!
) else (
    echo [警告] 未找到 START251.A51，编译将跳过启动文件
    echo        路径: %STARTUP_FILE%
)

REM ────── 步骤 3: 链接 → .ABS (OMF-251 绝对目标文件) ──────
echo   L251: 链接生成 output.ABS
L251.EXE !OBJ_LIST! TO("%OUTDIR%\output.ABS") RS(256) PL(68) PW(78) 2>&1
if errorlevel 1 (
    echo [错误] 链接失败
    exit /b 1
)

REM ────── 步骤 4: 生成 HEX ──────
echo   OH251: 生成 output.HEX
OH251.EXE "%OUTDIR%\output.ABS" HEXFILE("output.HEX") 2>&1
if errorlevel 1 (
    echo [错误] HEX 生成失败
    exit /b 1
)

echo ================================================
echo [完成] 编译成功！输出文件: output.HEX
echo ================================================