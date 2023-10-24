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

void TextLabel::CalculateOffset() {
	// reset variables
	posOffset = { 0.f, 0.f };
	/*float ymax{};
	float ymin{};*/
	float verticalPadding = 0.f;

	std::string::const_iterator c;
	for (c = textString.begin(); c != textString.end(); c++)
	{
		Character ch{ (*font).characters[*c] };

		float w = ch.size.x * relFontSize;
		float h = ch.size.y * relFontSize;

		// calculate ymax, ymin
		/*ymax = std::max(ymax, h);
		ymin = std::min(ymin, (ch.size.y - ch.bearing.y) * relFontSize);*/
		if (((ch.bearing.y - ch.size.y) < 0.f) && (verticalPadding == 0.f)) {
			DEBUG_PRINT("PADDING ADDED");
			verticalPadding = -(ch.bearing.y - ch.size.y);
		}

		// calculate size needed
		posOffset.x += w;
		posOffset.y = std::max(posOffset.y, h);

		if (c != std::prev(textString.end())) {
			posOffset.x += 0.5f * (ch.advance >> 6) * relFontSize;
		}
	}

	//finalise y-offset
	posOffset.y += verticalPadding;
}

void TextLabel::UpdateOffset(Vec2& relTransform, Transform const& transformData) {
	CalculateOffset();
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
			modelData.SetColor(defaultColor.r, defaultColor.g, defaultColor.b);
		}
	}
}

void TextLabel::OnClick(Model& modelData, Name& nameData) {
	//change color based Name->selected bool state
	DEBUG_PRINT("UI_ONCLICK");
	modelData.SetColor(focusedColor.r, focusedColor.g, focusedColor.b);
}

void TextLabel::OnHover(Model& modelData, Name& nameData) {
	//read Transform pos and mailbox mouse move
	// pos match, change color (probably more for button and not text...?)
	//DEBUG_PRINT("UI_ONHOVER");
	modelData.SetColor(hoveredColor.r, hoveredColor.g, hoveredColor.b);

}

void TextLabel::OnFocus() {
	DEBUG_PRINT("UI_ONFOCUS");
	// open properties perhaps?
	// likely to trigger together with onClick
}