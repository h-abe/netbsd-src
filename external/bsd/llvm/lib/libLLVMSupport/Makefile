#	$NetBSD: Makefile,v 1.26 2014/03/04 21:05:06 joerg Exp $

LIB=	LLVMSupport

NOGCCERROR=	yes

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/lib/Support

SRCS+=	APFloat.cpp \
	APInt.cpp \
	APSInt.cpp \
	ARMBuildAttrs.cpp \
	Allocator.cpp \
	BlockFrequency.cpp \
	BranchProbability.cpp \
	circular_raw_ostream.cpp \
	CommandLine.cpp \
	Compression.cpp \
	ConstantRange.cpp \
	ConvertUTF.c \
	ConvertUTFWrapper.cpp \
	CrashRecoveryContext.cpp \
	DataExtractor.cpp \
	DataStream.cpp \
	Debug.cpp \
	DeltaAlgorithm.cpp \
	DAGDeltaAlgorithm.cpp \
	Dwarf.cpp \
	ErrorHandling.cpp \
	FileOutputBuffer.cpp \
	FileUtilities.cpp \
	FoldingSet.cpp \
	FormattedStream.cpp \
	GraphWriter.cpp \
	Hashing.cpp \
	IntEqClasses.cpp \
	IntervalMap.cpp \
	IntrusiveRefCntPtr.cpp \
	IsInf.cpp \
	IsNAN.cpp \
	LEB128.cpp \
	LineIterator.cpp \
	Locale.cpp \
	LockFileManager.cpp \
	MD5.cpp \
	ManagedStatic.cpp \
	MemoryBuffer.cpp \
	MemoryObject.cpp \
	PluginLoader.cpp \
	PrettyStackTrace.cpp \
	Regex.cpp \
	SmallPtrSet.cpp \
	SmallVector.cpp \
	SourceMgr.cpp \
	Statistic.cpp \
	StreamableMemoryObject.cpp \
	StringExtras.cpp \
	StringMap.cpp \
	StringPool.cpp \
	StringRef.cpp \
	StringRefMemoryObject.cpp \
	SystemUtils.cpp \
	TargetRegistry.cpp \
	Timer.cpp \
	ToolOutputFile.cpp \
	Triple.cpp \
	Twine.cpp \
	Unicode.cpp \
	Watchdog.cpp \
	YAMLParser.cpp \
	YAMLTraits.cpp \
	raw_os_ostream.cpp \
	raw_ostream.cpp \
	regcomp.c \
	regerror.c \
	regexec.c \
	regfree.c \
	regstrlcpy.c

SRCS+=	Atomic.cpp \
	Disassembler.cpp \
	DynamicLibrary.cpp \
	Errno.cpp \
	Host.cpp \
	IncludeFile.cpp \
	Memory.cpp \
	Mutex.cpp \
	Path.cpp \
	Process.cpp \
	Program.cpp \
	RWMutex.cpp \
	SearchForAddressOfSpecialSymbol.cpp \
	Signals.cpp \
	system_error.cpp \
	ThreadLocal.cpp \
	Threading.cpp \
	TimeValue.cpp \
	Valgrind.cpp

.PATH: ${LLVM_SRCDIR}/lib/Support/Unix

SRCS+=	Host.inc \
	Memory.inc \
	Mutex.inc \
	Path.inc \
	Process.inc \
	Program.inc \
	RWMutex.inc \
	Signals.inc \
	Watchdog.inc \
	system_error.inc \
	ThreadLocal.inc \
	TimeValue.inc

.for src in ${SRCS:M*.c}
CPPFLAGS.${src}+=	-std=gnu99
.endfor

.if !defined(HOSTLIB) && !empty(MACHINE_ARCH:M*arm*)
# Missing __sync_fetch_and_add_4
# Missing __sync_fetch_and_sub_4
# Missing __sync_lock_test_and_set_4
# Missing __sync_val_compare_and_swap_4
# Missing __atomic_fetch_add_4
COPTS.Atomic.cpp+= -march=armv6
.endif

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
