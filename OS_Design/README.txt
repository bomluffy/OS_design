define.h kernel.c 生成.lib
T.c生成.exe

OS.c是所有代码汇总的，便于测试

生成.lib命令:g++ kernel.c -shared -o kernel.dll -Wl,--out-implib,kernel.lib
生成.exe命令:g++ T.c kernel.lib -o T.exe