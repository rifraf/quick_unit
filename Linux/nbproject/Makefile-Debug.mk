#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/194468644/vcl.o \
	${OBJECTDIR}/Linux.o

# Test Directory
TESTDIR=build/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/linux

dist/Debug/GNU-Linux-x86/linux: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/linux ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/194468644/vcl.o: ../code_under_test/vcl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/194468644
	$(COMPILE.cc) -g -o ${OBJECTDIR}/_ext/194468644/vcl.o ../code_under_test/vcl.cpp

${OBJECTDIR}/Linux.o: Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Linux.o Linux.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f1: ${TESTDIR}/VCLTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/VCLTests.o: VCLTests.cpp 
	${MKDIR} -p ${TESTDIR}
	$(COMPILE.cc) -g -I. -o ${TESTDIR}/VCLTests.o VCLTests.cpp


${OBJECTDIR}/_ext/194468644/vcl_nomain.o: ${OBJECTDIR}/_ext/194468644/vcl.o ../code_under_test/vcl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/194468644
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/194468644/vcl.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    $(COMPILE.cc) -g -Dmain=__nomain -o ${OBJECTDIR}/_ext/194468644/vcl_nomain.o ../code_under_test/vcl.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/194468644/vcl.o ${OBJECTDIR}/_ext/194468644/vcl_nomain.o;\
	fi

${OBJECTDIR}/Linux_nomain.o: ${OBJECTDIR}/Linux.o Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Linux.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    $(COMPILE.cc) -g -Dmain=__nomain -o ${OBJECTDIR}/Linux_nomain.o Linux.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Linux.o ${OBJECTDIR}/Linux_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/linux

# Subprojects
.clean-subprojects:
