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
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/IndividualsFileReporter.o \
	${OBJECTDIR}/PkPdReporter.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-static -static-libgcc -static-libstdc++
CXXFLAGS=-static -static-libgcc -static-libstdc++

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../MalariaCore/dist/Release/MinGW-Windows/libmalariacore.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ec50_efficacytablegenerator.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ec50_efficacytablegenerator.exe: ../MalariaCore/dist/Release/MinGW-Windows/libmalariacore.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ec50_efficacytablegenerator.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ec50_efficacytablegenerator ${OBJECTFILES} ${LDLIBSOPTIONS} -l gsl -l gslcblas -l yaml-cpp

${OBJECTDIR}/IndividualsFileReporter.o: IndividualsFileReporter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../MalariaCore -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IndividualsFileReporter.o IndividualsFileReporter.cpp

${OBJECTDIR}/PkPdReporter.o: PkPdReporter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../MalariaCore -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PkPdReporter.o PkPdReporter.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../MalariaCore -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:
	cd ../MalariaCore && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ec50_efficacytablegenerator.exe

# Subprojects
.clean-subprojects:
	cd ../MalariaCore && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
