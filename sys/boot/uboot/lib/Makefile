# $FreeBSD$

.include <src.opts.mk>

.PATH: ${.CURDIR}/../../common

LIB=		uboot
INTERNALLIB=
WARNS?=		2

SRCS=	console.c copy.c devicename.c elf_freebsd.c glue.c
SRCS+=	module.c net.c reboot.c time.c

CFLAGS+=	-ffreestanding -msoft-float

.if !defined(LOADER_NO_DISK_SUPPORT)
SRCS+=	disk.c
CFLAGS+= -DLOADER_DISK_SUPPORT
.endif

.if ${MK_FDT} != "no"
LOADER_FDT_SUPPORT=	yes
.else
LOADER_FDT_SUPPORT=	no
.endif

.if ${LOADER_FDT_SUPPORT} == "yes"
CFLAGS+= -DLOADER_FDT_SUPPORT -I${.CURDIR}/../../fdt
.endif

# Pick up FDT includes
CFLAGS+=	-I${.CURDIR}/../../../../sys/contrib/libfdt/

# Pick up the bootstrap header for some interface items
CFLAGS+=	-I${.CURDIR}/../../common -I${.CURDIR}/../../.. -I.

.ifdef(BOOT_DISK_DEBUG)
# Make the disk code more talkative
CFLAGS+= -DDISK_DEBUG
.endif

machine: .NOMETA
	ln -sf ${.CURDIR}/../../../${MACHINE_CPUARCH}/include machine

CLEANFILES+=	machine

.include <bsd.stand.mk>
.include <bsd.lib.mk>

beforedepend ${OBJS}: machine
