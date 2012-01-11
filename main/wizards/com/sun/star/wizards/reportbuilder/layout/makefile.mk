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



PRJ		= ..$/..$/..$/..$/..$/..
PRJNAME = wizards
TARGET  = reportbuilderwizardlayouts
PACKAGE = com$/sun$/star$/wizards$/reportbuilder$/layout

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
#.INCLUDE :  $(PRJ)$/util$/makefile.pmk

JARFILES= unoil.jar jurt.jar ridl.jar juh.jar java_uno.jar commonwizards.jar report.jar

# CUSTOMMANIFESTFILE= MANIFEST.MF

JARCLASSDIRS	= \
    com$/sun$/star$/wizards$/reportbuilder$/layout \
	com$/sun$/star$/wizards$/report

# JARTARGET	= $(TARGET).jar
# JARCLASSPATH = commonwizards.jar report.jar

# --- Files --------------------------------------------------------

JAVAFILES=                     \
	LayoutConstants.java       \
	ReportBuilderLayouter.java \
	Tabular.java               \
	ColumnarSingleColumn.java  \
	ColumnarTwoColumns.java    \
	ColumnarThreeColumns.java  \
	InBlocksLabelsAbove.java   \
	InBlocksLabelsLeft.java    \
	SectionObject.java         \
	SectionEmptyObject.java    \
	SectionLabel.java          \
	SectionTextField.java      \
	DesignTemplate.java

JAVACLASSFILES = $(foreach,i,$(JAVAFILES) $(CLASSDIR)$/$(PACKAGE)$/$(i:b).class)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

