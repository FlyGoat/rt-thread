import os

if os.getenv('RTT_ROOT'):
    RTT_ROOT = os.getenv('RTT_ROOT')
else:
    RTT_ROOT = '../..'

# CPU options
ARCH='mips'
CPU ='common'

# toolchains options
CROSS_TOOL  = 'gcc'

if os.getenv('RTT_CC'):
	CROSS_TOOL = os.getenv('RTT_CC')

if  CROSS_TOOL == 'gcc':
	PLATFORM    = 'gcc'
	EXEC_PATH   = "/opt/mips-mti-elf/2019.09-01/bin"
#	EXEC_PATH   = r'D:\mgc\embedded\codebench\bin'
else:
    print('================ERROR===========================')
    print('Not support %s yet!' % CROSS_TOOL)
    print('=================================================')
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
	EXEC_PATH = os.getenv('RTT_EXEC_PATH')

BUILD       = 'debug'

# don't use loongson company's cross-compilation tool chain to compile the RT-Thread
# must use the cross-compilation tool chain that RT-Thread recommand
# download: https://coding.net/u/bernard/p/rtthread_tools/git/blob/master/GCC_Toolchains.md
PREFIX = 'mips-mti-elf-'
CC = PREFIX + 'gcc'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
LINK = PREFIX + 'gcc'
TARGET_EXT = 'elf'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'
READELF = PREFIX + 'readelf'

DEVICE = ' -mips32r2 -msoft-float -mfp32'
CFLAGS = DEVICE + ' -EL -G0 -mno-abicalls -fno-pic -fno-builtin -fno-exceptions -ffunction-sections -fomit-frame-pointer -D__PTHREAD_h -D_SYS__PTHREADTYPES_H_ -D_SCHED_H_ -D_SYS_SCHED_H_'
AFLAGS = ' -c' + DEVICE + ' -EL -fno-pic -fno-builtin -mno-abicalls -x assembler-with-cpp'
LFLAGS = DEVICE + ' -nostartfiles -EL -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,Reset_Handler -T mipssim_ram.lds'

CPATH = ''
LPATH = ''

if BUILD == 'debug':
    CFLAGS += ' -O0 -gdwarf-2'
    AFLAGS += ' -gdwarf-2'
else:
    CFLAGS += ' -O2'

DUMP_ACTION = OBJDUMP + ' -D -S $TARGET > rtt.asm\n'
READELF_ACTION = READELF + ' -a $TARGET > rtt.map\n'
POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'
