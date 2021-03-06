/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/


#ifndef _MERGETBL_HXX
#define _MERGETBL_HXX

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#include <svx/stddlg.hxx>


class SwMergeTblDlg : public SvxStandardDialog
{
	OKButton 		aOKPB;
	CancelButton 	aCancelPB;
	HelpButton 		aHelpPB;
    FixedLine        aMergeFL;
	RadioButton		aMergePrevRB;
	RadioButton		aMergeNextRB;

	sal_Bool& 			rMergePrev;
protected:
	virtual void Apply();

public:
	SwMergeTblDlg( Window *pParent, sal_Bool& rWithPrev );
};

#endif


