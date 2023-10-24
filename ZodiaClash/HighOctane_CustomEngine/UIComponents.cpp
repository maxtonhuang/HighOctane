/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		UIComponents.cpp
*
*	@co-author	Chua Zhen Rong
*
*	@email		c.zhenrong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		23 October 2023
*
* *****************************************************************************
*
*	@brief		UI components used for in-game UI
*
*	UIComponents contianing UI components with their own niche properties
*	to be set and modified.
*
******************************************************************************/

#include "UIComponents.h"
#include "message.h"
#include "Global.h"
#include "Utilities.h"
#include "Font.h"

vmath::Vector2 uiMousePos{ RESET_VEC2 };
vmath::Vector2 uiOffset{ RESET_VEC2 };

TextLabel::TextLabel() {
	font = fonts.GetDefaultFont();
	textString = "TextLabel";
	textAlignment = UI_HORIZONTAL_ALIGNMENT::H_LEFT_ALIGN;
	relFontSize = 0.5f;
	defaultColor = { 1.f, 1.f, 1.f, 1.f };
	hoveredColor = { 1.f, 0.f, 0.f, 1.f };
	focusedColor = { 0.f, 0.f, 1.f, 1.f };
}

TextLabel::TextLabel(Font& f, std::string str, UI_HORIZONTAL_ALIGNMENT align) {
	font = &f; 
	textString = str; 
	textAlignment = align;
	relFontSize = 0.5f;
	defaultColor = { 1.f, 1.f, 1.f, 1.f };
	hoveredColor = { 1.f, 0.f, 0.f, 1.f };
	focusedColor = { 0.f, 0.f, 1.f, 1.f };
}

TextLabel::TextLabel(std::string str, UI_HORIZONTAL_ALIGNMENT align) {
	font = fonts.GetDefaultFont();
	textString = str;
	textAlignment = align;
	relFontSize = 0.5f;
	defaultColor = { 1.f, 1.f, 1.f, 1.f };
	hoveredColor = { 1.f, 0.f, 0.f, 1.f };
	focusedColor = { 0.f, 0.f, 1.f, 1.f };
}

bool TextLabel::CheckStringUpdated(TextLabel& txtLblData) {
	if (txtLblData.textString != txtLblData.prevTextString) {
		prevTextString = txtLblData.textString;
		return true;
	}
	return false;
}

void TextLabel::SetTextString(std::string txtStr) {
	textString = txtStr;
}

void TextLabel::CalculateOffset(Size& sizeData) {
	//DEBUG_PRINT("Recalculating...");
	// reset variables
	posOffset = { 0.f, 0.f };
	float verticalPadding = static_cast<float>(-(*font).largestNegativeOffset);

	std::string::const_iterator c;
	for (c = textString.begin(); c != textString.end(); c++)
	{
		Character ch{ (*font).characters[*c] };

		float w = ch.size.x * relFontSize;
		float h = ch.size.y * relFontSize;

		// calculate size needed
		if (*c != textString[textString.size() - 1]) {
			posOffset.x += ((ch.advance >> 6) * relFontSize) - w;
		}
		posOffset.x += (w + (ch.bearing.x * relFontSize));
		posOffset.y = std::max(posOffset.y, h);		
	}

	//finalise y-offset
	posOffset.y += verticalPadding;

	sizeData.width = posOffset.x;
	sizeData.height = posOffset.y;
}

void TextLabel::UpdateOffset(Transform const& transformData, Size& sizeData) {
	CalculateOffset(sizeData);
	relTransform.x = (transformData.position.x - (0.5f * posOffset.x));
	relTransform.y = (transformData.position.y - (0.25f * posOffset.y));
}

void TextLabel::IsClickedOrHovered(Transform& transformData, Model& modelData, Name& nameData) {
	// get cursorPos, compare with pos in Transform, return if no match

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::UICOMPONENT]) {
		switch (msg.type) {
		case(TYPE::MOUSE_MOVE):
			uiMousePos = { msg.posX, msg.posY };
			break;
		case(TYPE::MOUSE_CLICK):
			if (IsWithinObject(modelData, uiMousePos)) {
				uiOffset = GetOffset(transformData.position, uiMousePos);
				OnClick(modelData, nameData);
			}
			break;
		case(TYPE::MOUSE_DOWN):
			if (nameData.selected) {
				transformData.position.x = msg.posX;
				transformData.position.y = msg.posY;
			}
			break;
		}		
	}

	if (!nameData.selected) {
		if (IsWithinObject(modelData, uiMousePos)) {
			OnHover(modelData, nameData);
		}
		else {
			SetTextString("TextString");
			modelData.SetColor(defaultColor.r, defaultColor.g, defaultColor.b);
		}
	}
}

void TextLabel::OnClick(Model& modelData, Name& nameData) {
	//change color based Name->selected bool state
	//DEBUG_PRINT("UI_ONCLICK");
	modelData.SetColor(focusedColor.r, focusedColor.g, focusedColor.b);
}

void TextLabel::OnHover(Model& modelData, Name& nameData) {
	//read Transform pos and mailbox mouse move
	// pos match, change color (probably more for button and not text...?)
	//DEBUG_PRINT("UI_ONHOVER");
	SetTextString("Hovered Text");
	modelData.SetColor(hoveredColor.r, hoveredColor.g, hoveredColor.b);

}

void TextLabel::OnFocus() {
	SetTextString("Focused Text");
	//DEBUG_PRINT("UI_ONFOCUS");
	// open properties perhaps?
	// likely to trigger together with onClick
}