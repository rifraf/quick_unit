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
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/194468644/vcl.o \
	${OBJECTDIR}/MinGW.o

# Test Directory
TESTDIR=build/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f2 \
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
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/MinGW-Windows/mingw_netbeans.exe

dist/Debug/MinGW-Windows/mingw_netbeans.exe: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/MinGW-Windows
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mingw_netbeans ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/194468644/vcl.o: ../code_under_test/vcl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/194468644
	$(COMPILE.cc) -g -o ${OBJECTDIR}/_ext/194468644/vcl.o ../code_under_test/vcl.cpp

${OBJECTDIR}/MinGW.o: MinGW.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -o ${OBJECTDIR}/MinGW.o MinGW.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f2: ${TESTDIR}/tests/UltraSlim.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f2 $^ ${LDLIBSOPTIONS} 

${TESTDIR}/TestFiles/f1: ${TESTDIR}/VCLTests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/tests/UltraSlim.o: tests/UltraSlim.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	$(COMPILE.cc) -g -I. -o ${TESTDIR}/tests/UltraSlim.o tests/UltraSlim.cpp


${TESTDIR}/VCLTests.o: VCLTests.cpp 
	${MKDIR} -p ${TESTDIR}
	$(COMPILE.cc) -g -I. -I. -o ${TESTDIR}/VCLTests.o VCLTests.cpp


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

${OBJECTDIR}/MinGW_nomain.o: ${OBJECTDIR}/MinGW.o MinGW.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/MinGW.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    $(COMPILE.cc) -g -Dmain=__nomain -o ${OBJECTDIR}/MinGW_nomain.o MinGW.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/MinGW.o ${OBJECTDIR}/MinGW_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f2 || true; \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/MinGW-Windows/mingw_netbeans.exe

# Subprojects
.clean-subprojects:
