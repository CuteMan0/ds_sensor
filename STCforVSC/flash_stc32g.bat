@echo off
chcp 65001 >nul
REM ═══════════════════════════════════════════════════════
REM  STC32G12K128 烧录脚本 (STC-ISP)
REM
REM  使用方法:
REM    1. 修改下方 STC_ISP_PATH 为你的 STC-ISP.exe 路径
REM    2. 修改 COM_PORT 为你的 USB-TTL 串口号
REM    3. 芯片冷启动后运行此脚本，或运行 build_stc32g.bat 后执行
REM
REM  注意: STC-ISP 命令行参数可能因版本而异，
REM        请先用 STC-ISP /? 查看你的版本支持的命令行参数
REM ═══════════════════════════════════════════════════════

REM ────── 配置区 (请根据实际情况修改) ──────
set STC_ISP_EXE=C:\STC-ISP\stc-isp-15xx-v6.94H.exe
set COM_PORT=COM3
set MCU_TYPE=STC32G12K128
set IRC_FREQ=35.000
set HEX_FILE=output.HEX
REM ──────────────────────────────────────────────

echo [烧录] STC32G12K128 温湿度检测项目
echo ================================================

if not exist "%HEX_FILE%" (
    echo [错误] 未找到 HEX 文件: %HEX_FILE%
    echo        请先运行 build_stc32g.bat 编译项目
    exit /b 1
)

if not exist "%STC_ISP_EXE%" (
    echo [错误] 未找到 STC-ISP: %STC_ISP_EXE%
    echo        请修改 flash_stc32g.bat 中的 STC_ISP_EXE 路径
    exit /b 1
)

echo   HEX 文件: %HEX_FILE%
echo   目标串口: %COM_PORT%
echo   MCU 型号: %MCU_TYPE%
echo   目标频率: %IRC_FREQ% MHz
echo ================================================
echo [提示] 请确认芯片已断电，然后重新上电（冷启动）
echo        脚本将自动尝试下载...
echo ================================================

REM ────── STC-ISP 命令行自动下载 ──────
REM 不同版本的 STC-ISP 命令行格式可能不同，以下是常见格式:
REM
REM   新版本 (v6.91+):
REM   stc-isp-15xx-v6.xx.exe /d=<MCU> /f=<HEX> /p=<COM> /t=<freq>
REM
REM   旧版本:
REM   stc-isp-15xx-v6.xx.exe -d <MCU> -f <HEX> -p <COM> -t <freq>

"%STC_ISP_EXE%" /d=%MCU_TYPE% /f=%HEX_FILE% /p=%COM_PORT% /t=%IRC_FREQ%

if errorlevel 1 (
    echo [警告] STC-ISP 返回错误码，请检查:
    echo        1. 芯片是否正确连接
    echo        2. COM 口号是否正确 (%COM_PORT%)
    echo        3. 芯片是否已执行冷启动（断电→上电）
) else (
    echo [完成] 烧录成功！
)

echo ================================================