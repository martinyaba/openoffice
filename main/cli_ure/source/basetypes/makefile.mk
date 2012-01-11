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



PRJ = ..$/..
PRJNAME = cli_ure

# for dummy
TARGET = basetypes

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/util$/makefile.pmk

.INCLUDE : $(PRJ)$/util$/target.pmk
.INCLUDE : target.mk

.IF "$(BUILD_FOR_CLI)" != ""

.INCLUDE : $(BIN)$/cliureversion.mk

ASSEMBLY_ATTRIBUTES = $(MISC)$/assembly_ure_$(TARGET).cs
POLICY_ASSEMBLY_FILE=$(BIN)$/$(CLI_BASETYPES_POLICY_ASSEMBLY).dll

ALLTAR : \
	$(BIN)$/cli_basetypes.dll \
	$(POLICY_ASSEMBLY_FILE)
	
.IF "$(CCNUMVER)" >= "001399999999"
CSCFLAGS+=-keyfile:"$(BIN)$/cliuno.snk"
.ENDIF

CSFILES = \
	uno$/Any.cs			\
	uno$/BoundAttribute.cs \
	uno$/ExceptionAttribute.cs	\
	uno$/ParameterizedTypeAttribute.cs	\
	uno$/TypeParametersAttribute.cs \
	uno$/TypeArgumentsAttribute.cs \
	uno$/OnewayAttribute.cs	\
	uno$/PolymorphicType.cs \
	$(ASSEMBLY_ATTRIBUTES)

.IF "$(CCNUMVER)" <= "001399999999"
$(ASSEMBLY_ATTRIBUTES) : assembly.cs makefile.mk $(BIN)$/cliuno.snk $(BIN)$/cliureversion.mk 
    $(GNUCOPY) -p assembly.cs $@
    echo \
    '[assembly:System.Reflection.AssemblyVersion( "$(CLI_BASETYPES_NEW_VERSION)")] \
    [assembly:System.Reflection.AssemblyKeyFile(@"$(BIN)$/cliuno.snk")]' \
    >> $@
.ELSE
$(ASSEMBLY_ATTRIBUTES) : assembly.cs makefile.mk $(BIN)$/cliuno.snk $(BIN)$/cliureversion.mk 
    $(GNUCOPY) -p assembly.cs $@
    echo \
    '[assembly:System.Reflection.AssemblyVersion( "$(CLI_BASETYPES_NEW_VERSION)")]' \
    >> $@
.ENDIF

$(BIN)$/cli_basetypes.dll : $(CSFILES) $(BIN)$/cliureversion.mk 
	$(CSC) $(CSCFLAGS) \
		-target:library \
		-out:$@ \
		-reference:System.dll \
		$(CSFILES)
	@echo "If code has changed then provide a policy assembly and change the version!"

#do not forget to deliver cli_uretypes.config. It is NOT embedded in the policy file.
$(POLICY_ASSEMBLY_FILE) : $(BIN)$/cli_basetypes.config
	$(WRAPCMD) AL.exe -out:$@ \
			-version:$(CLI_BASETYPES_POLICY_VERSION) \
			-keyfile:$(BIN)$/cliuno.snk \
			-link:$(BIN)$/cli_basetypes.config


#Create the config file that is used with the policy assembly
$(BIN)$/cli_basetypes.config: cli_basetypes_config $(BIN)$/cliureversion.mk 
	$(PERL) $(SOLARENV)$/bin$/clipatchconfig.pl \
	$< $@


.ENDIF
	

