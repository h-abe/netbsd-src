#	$NetBSD: Makefile,v 1.6 2014/01/15 22:21:34 joerg Exp $

LIB=	LLVMIR

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/lib/IR

SRCS+=	AsmWriter.cpp \
	Attributes.cpp \
	AutoUpgrade.cpp \
	BasicBlock.cpp \
	ConstantFold.cpp \
	Constants.cpp \
	Core.cpp \
	DIBuilder.cpp \
	DataLayout.cpp \
	DebugInfo.cpp \
	DebugLoc.cpp \
	DiagnosticInfo.cpp \
	DiagnosticPrinter.cpp \
	Dominators.cpp \
	Function.cpp \
	GCOV.cpp \
	GVMaterializer.cpp \
	Globals.cpp \
	IRBuilder.cpp \
	IRPrintingPasses.cpp \
	InlineAsm.cpp \
	Instruction.cpp \
	Instructions.cpp \
	IntrinsicInst.cpp \
	LLVMContext.cpp \
	LLVMContextImpl.cpp \
	LeakDetector.cpp \
	LegacyPassManager.cpp \
	Mangler.cpp \
	Metadata.cpp \
	Module.cpp \
	Pass.cpp \
	PassManager.cpp \
	PassRegistry.cpp \
	Type.cpp \
	TypeFinder.cpp \
	Use.cpp \
	User.cpp \
	Value.cpp \
	ValueSymbolTable.cpp \
	ValueTypes.cpp \
	Verifier.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
