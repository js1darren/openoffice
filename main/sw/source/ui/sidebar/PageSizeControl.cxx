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



#include "precompiled_sw.hxx"

#include "PageSizeControl.hxx"
#include "PagePropertyPanel.hxx"
#include "PagePropertyPanel.hrc"

#include <cmdid.h>
#include <swtypes.hxx>

#include <svx/sidebar/ValueSetWithTextControl.hxx>

#include <tools/inetmime.hxx>
#include <editeng/paperinf.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>


namespace sw { namespace sidebar {

PageSizeControl::PageSizeControl(
	Window* pParent,
	PagePropertyPanel& rPanel,
	const Paper ePaper,
	const sal_Bool bLandscape,
	const FieldUnit eFUnit )
	: ::svx::sidebar::PopupControl( pParent, SW_RES(RID_POPUP_SWPAGE_SIZE) )
	, mpSizeValueSet( new ::svx::sidebar::ValueSetWithTextControl( ::svx::sidebar::ValueSetWithTextControl::TEXT_TEXT, this, SW_RES(VS_SIZE) ) )
	, maMoreButton( this, SW_RES(CB_SIZE_MORE) )
	, maWidthHeightField( this, SW_RES(FLD_WIDTH_HEIGHT) )
	, mePaper( ePaper )
	, maPaperList()
	, mrPagePropPanel(rPanel)
{
	maWidthHeightField.Hide();
	SetFieldUnit( maWidthHeightField, eFUnit );

	maPaperList.push_back( PAPER_A3 );
	maPaperList.push_back( PAPER_A4 );
	maPaperList.push_back( PAPER_A5 );
	maPaperList.push_back( PAPER_B4_ISO );
	maPaperList.push_back( PAPER_B5_ISO );
	maPaperList.push_back( PAPER_ENV_C5 );
	maPaperList.push_back( PAPER_LETTER );
	maPaperList.push_back( PAPER_LEGAL );

	mpSizeValueSet->SetStyle( mpSizeValueSet->GetStyle() | WB_3DLOOK | WB_NO_DIRECTSELECT );
	mpSizeValueSet->SetColor( GetSettings().GetStyleSettings().GetMenuColor() );

	sal_uInt16 nSelectedItem = 0;
	{
		XubString aMetricStr;
		{
			const XubString aText = maWidthHeightField.GetText();
			for (short i = aText.Len() - 1; i >= 0; i--)
			{
				xub_Unicode c = aText.GetChar(i);
				if ( INetMIME::isAlpha(c) || (c == '\'') || (c == '\"') || (c == '%') )
				{
					aMetricStr.Insert(c, 0);
				}
				else
				{
					if (aMetricStr.Len())
					{
						break;
					}
				}
			}
		}

		const LocaleDataWrapper& localeDataWrapper = maWidthHeightField.GetLocaleDataWrapper();
		String WidthStr;
		String HeightStr;
		String ItemText2;
		for ( ::std::vector< Paper >::size_type nPaperIdx = 0;
			  nPaperIdx < maPaperList.size();
			  ++nPaperIdx )
		{
			Size aPaperSize = SvxPaperInfo::GetPaperSize( maPaperList[ nPaperIdx ] );
			if ( bLandscape )
			{
				Swap( aPaperSize );
			}
			maWidthHeightField.SetValue( maWidthHeightField.Normalize( aPaperSize.Width() ), FUNIT_TWIP );
			WidthStr = localeDataWrapper.getNum(
				maWidthHeightField.GetValue(),
				maWidthHeightField.GetDecimalDigits(),
				maWidthHeightField.IsUseThousandSep(),
				maWidthHeightField.IsShowTrailingZeros() );

			maWidthHeightField.SetValue( maWidthHeightField.Normalize( aPaperSize.Height() ), FUNIT_TWIP);
			HeightStr = localeDataWrapper.getNum(
				maWidthHeightField.GetValue(),
				maWidthHeightField.GetDecimalDigits(),
				maWidthHeightField.IsUseThousandSep(),
				maWidthHeightField.IsShowTrailingZeros() );

			ItemText2 = WidthStr;
			ItemText2 += String::CreateFromAscii(" x ");
			ItemText2 += HeightStr;
			ItemText2 += String::CreateFromAscii(" ");
			ItemText2 += aMetricStr;

			mpSizeValueSet->AddItem(
				SvxPaperInfo::GetName( maPaperList[ nPaperIdx ] ),
				ItemText2,
				0 );

			if ( maPaperList[ nPaperIdx ] == mePaper )
			{
				nSelectedItem = nPaperIdx + 1;
			}
		}
	}

	mpSizeValueSet->SetNoSelection();
	mpSizeValueSet->SetSelectHdl( LINK(this, PageSizeControl,ImplSizeHdl ) );
	mpSizeValueSet->Show();

	mpSizeValueSet->SelectItem( nSelectedItem );
	mpSizeValueSet->Format();
	mpSizeValueSet->StartSelection();

	maMoreButton.SetClickHdl( LINK( this, PageSizeControl, MoreButtonClickHdl_Impl ) );
	maMoreButton.GrabFocus();

	FreeResource();
}


PageSizeControl::~PageSizeControl(void)
{
	delete mpSizeValueSet;
}


IMPL_LINK(PageSizeControl, ImplSizeHdl, void *, pControl)
{
	mpSizeValueSet->SetNoSelection();
	if ( pControl == mpSizeValueSet )
	{
		const sal_uInt16 nSelectedPaper = mpSizeValueSet->GetSelectItemId();
		const Paper ePaper = maPaperList[nSelectedPaper - 1];
		if ( ePaper != mePaper )
		{
			mePaper = ePaper;
			mrPagePropPanel.ExecuteSizeChange( mePaper );
		}
	}

	mrPagePropPanel.ClosePageSizePopup();
	return 0;
}

IMPL_LINK(PageSizeControl, MoreButtonClickHdl_Impl, void *, EMPTYARG)
{
	mrPagePropPanel.GetBindings()->GetDispatcher()->Execute( FN_FORMAT_PAGE_SETTING_DLG, SFX_CALLMODE_ASYNCHRON );

	mrPagePropPanel.ClosePageSizePopup();
	return 0;
}


} } // end of namespace sw::sidebar

/* vim: set noet sw=4 ts=4: */
