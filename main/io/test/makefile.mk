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



PRJ=..

PRJNAME=extensions
TARGET=workben
LIBTARGET=NO

TARGETTYPE=CUI
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.IF "$(L10N_framework)"==""
# --- Files --------------------------------------------------------

OBJFILES=	$(OBJ)$/testcomponent.obj \
		$(OBJ)$/testconnection.obj

UNOUCRDEP=$(SOLARBINDIR)$/udkapi.rdb
UNOUCRRDB=$(SOLARBINDIR)$/udkapi.rdb

# output directory (one dir for each project)
UNOUCROUT=$(OUT)$/inc

UNOTYPES =	com.sun.star.connection.XConnector \
		com.sun.star.connection.XAcceptor  \
		com.sun.star.registry.XImplementationRegistration \
		com.sun.star.lang.XComponent \
		com.sun.star.lang.XSingleServiceFactory \
		com.sun.star.lang.XMultiServiceFactory \
        com.sun.star.test.XSimpleTest            \
        com.sun.star.lang.XSingleComponentFactory \
        com.sun.star.lang.XMultiComponentFactory


#
# std testcomponent
#

APP1TARGET = testcomponent
APP1OBJS   = $(OBJ)$/testcomponent.obj
APP1STDLIBS = 	$(SALLIB) \
		$(CPPULIB)\
		$(CPPUHELPERLIB)

APP2TARGET = testconnection
APP2OBJS   = $(OBJ)$/testconnection.obj
APP2STDLIBS = 	$(SALLIB) \
		$(CPPULIB) \
		$(CPPUHELPERLIB)



# --- Targets ------------------------------------------------------

ALL : 	$(BIN)$/applicat.rdb	\
	ALLTAR

$(BIN)$/applicat.rdb: $(SOLARBINDIR)$/udkapi.rdb
	rm -f $@
	regmerge $@ / $?

.ENDIF 		# L10N_framework

.INCLUDE :  target.mk
