#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************


PRJ=..$/..

PRJNAME=extensions
TARGET=teststm
USE_DEFFILE=TRUE
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------
.INCLUDE :	$(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------
OBJFILES=	 $(SLO)$/pipetest.obj  $(SLO)$/testfactreg.obj $(SLO)$/datatest.obj \
					$(SLO)$/marktest.obj

SHL1TARGET= $(TARGET)$(DLLPOSTFIX)
LIB1TARGET= $(SLB)$/$(TARGET).lib	

SHL1STDLIBS= \
		$(VOSLIB) 	 \
		$(SALLIB) 	 \
		$(TOOLSLIB)	

LIB1OBJFILES=	$(OBJFILES)
SHL1LIBS=		$(LIB1TARGET)
SHL1IMPLIB=		i$(TARGET)
SHL1DEPN=		makefile.mk $(SHL1LIBS)
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def

DEF1NAME=		$(SHL1TARGET)
DEF1EXPORTFILE=	exports.dxp


# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk
.INCLUDE :	$(PRJ)$/util$/target.pmk
