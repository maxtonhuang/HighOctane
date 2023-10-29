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
#include "graphics.h"
#include "Colors.h"

vmath::Vector2 uiMousePos{ RESET_VEC2 };

/**************************
******** TEXTLABEL ********
**************************/

TextLabel::TextLabel() {
	font = fonts.GetDefaultFont();
	textString = "TextLabel";
	textAlignment = UI_HORIZONTAL_ALIGNMENT::H_CENTER_ALIGN;
	relFontSize = 0.5f;
	textColor = &colors.colorMap["black"];
}

TextLabel::TextLabel(std::string str, std::string txtColor) {
	font = fonts.GetDefaultFont();
	textString = str;
	textAlignment = UI_HORIZONTAL_ALIGNMENT::H_CENTER_ALIGN;
	relFontSize = 0.5f;
	textColor = &colors.colorMap[txtColor];
}

void TextLabel::SetTextString(std::string txtStr) {
	textString = txtStr;
}

bool TextLabel::CheckStringUpdated(TextLabel& txtLblData) {
	if (txtLblData.textString != txtLblData.prevTextString) {
		prevTextString = txtLblData.textString;
		return true;
	}
	return false;
}

void TextLabel::CalculateOffset() {
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

	textWidth = posOffset.x;
	textHeight = posOffset.y;
}

void TextLabel::UpdateOffset(Transform const& transformData) {
	CalculateOffset();
	relTransform.x = (transformData.position.x - (0.5f * posOffset.x));
	relTransform.y = (transformData.position.y - (0.25f * posOffset.y));
}

void TextLabel::Update(Model& modelData, Name& nameData) {
	// get cursorPos, compare with pos in Transform, return if no match
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::UICOMPONENT]) {
		switch (msg.type) {
		case(TYPE::MOUSE_MOVE):
			uiMousePos = { msg.posX, msg.posY };
			break;
		case(TYPE::MOUSE_CLICK):
			if (IsWithinObject(modelData, uiMousePos)) {
				//OnClick(modelData, nameData);
			}
			break;
		}		
	}

	if (nameData.selected) {
		//SetTextString("Focused Text");
		currentState = STATE::FOCUSED;
		//modelData.SetColor(focusedColor.r, focusedColor.g, focusedColor.b);
	}
	else if (IsWithinObject(modelData, uiMousePos)) {
		//SetTextString("Hovered Text");
		currentState = STATE::HOVERED;
		//modelData.SetColor(hoveredColor.r, hoveredColor.g, hoveredColor.b);
	}
	else {
		//SetTextString("TextString");
		currentState = STATE::NONE;
		//modelData.SetColor(defaultColor.r, defaultColor.g, defaultColor.b);
	}

	/*if (!nameData.selected && !transformData.isStatic) {
		if (IsWithinObject(modelData, uiMousePos)) {
			OnHover(modelData, nameData);
		}
		else {
			SetTextString("TextString");
			modelData.SetColor(defaultColor.r, defaultColor.g, defaultColor.b);
		}
	}*/
}

//void TextLabel::OnClick(Model& modelData, Name& nameData) {
//	//change color based Name->selected bool state
//	//DEBUG_PRINT("UI_ONCLICK");
//	//modelData.SetColor(focusedColor.r, focusedColor.g, focusedColor.b);
//}
//
//void TextLabel::OnHover(Model& modelData/*, Name& nameData*/) {
//	//read Transform pos and mailbox mouse move
//	// pos match, change color (probably more for button and not text...?)
//	//DEBUG_PRINT("UI_ONHOVER");
//	SetTextString("Hovered Text");
//	modelData.SetColor(hoveredColor.r, hoveredColor.g, hoveredColor.b);
//
//}
//
//void TextLabel::OnFocus(Model& modelData) {
//	SetTextString("Focused Text");
//	modelData.SetColor(focusedColor.r, focusedColor.g, focusedColor.b);
//	//DEBUG_PRINT("UI_ONFOCUS");
//	// open properties perhaps?
//	// likely to trigger together with onClick
//}



/**************************
********* BUTTON **********
**************************/

Button::Button() {
	std::string txtStr = "TextString";
	std::string btnColor = "white";
	std::string txtColor = "blue";
	defaultColor = { btnColor, txtColor };
	hoveredColor = { txtColor, btnColor };
	focusedColor = { txtColor, btnColor };
	/*eventName = "Audio";
	eventTrigger = functions[eventName];
	eventInput = "ping.wav";*/
}

Button::Button(std::string btnColor, glm::vec4* txtColor) {
	std::string txtStr = "TextString";
	defaultColor = { btnColor, txtColor };
	hoveredColor = { txtColor, btnColor };
	focusedColor = { txtColor, btnColor };
	/*eventName = "Audio";
	eventTrigger = functions[eventName];
	eventInput = "ping.wav";*/
}

glm::vec4* Button::GetButtonColor() {
	switch (currentState) {
	case(STATE::HOVERED):
		return hoveredColor.buttonColor;
		break;
	case(STATE::FOCUSED):
		return focusedColor.buttonColor;
		break;
	default:
		return defaultColor.buttonColor;
		break;
	}
}

void Button::Update(Model& modelData, Name& nameData, TextLabel& textLabelData) {
	// get cursorPos, compare with pos in Transform, return if no match
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::UICOMPONENT]) {
		switch (msg.type) {
		case(TYPE::MOUSE_MOVE):
			uiMousePos = { msg.posX, msg.posY };
			break;
		case(TYPE::MOUSE_CLICK):
			if (IsWithinObject(modelData, uiMousePos)) {
				//on click event trigger (outside edit mode)
				if (!edit_mode && !eventName.empty() && !eventInput.empty()) {
					events.Call(eventName, eventInput);
					//eventTrigger(eventInput);
				}

				//DEBUG_PRINT("btnWidth: %f , btnHeight: %f", buttonWidth, buttonHeight);
				//DEBUG_PRINT("txtWidth: %f , txtHeight: %f", textLabelData.textWidth, textLabelData.textHeight);

				//test updateColor
				/*glm::vec4 newColor = { 0.09f, 0.63f, 0.72f, 1.0f };
				colors.UpdateColor("blue", newColor);*/
			}
			break;
		}
	}

	// update button and text colors
	if (nameData.selected) {
		//textLabel.SetTextString("Focused Text");
		//modelData.SetColor(focusedColor.buttonColor->r, focusedColor.buttonColor->g, focusedColor.buttonColor->b);
		textLabelData.textColor = focusedColor.textColor;
		currentState = STATE::FOCUSED;		
	}
	else if (IsWithinObject(modelData, uiMousePos)) {
		//textLabel.SetTextString("Hovered Text");
		//modelData.SetColor(hoveredColor.buttonColor->r, hoveredColor.buttonColor->g, hoveredColor.buttonColor->b);
		textLabelData.textColor = hoveredColor.textColor;
		currentState = STATE::HOVERED;		
	}
	else {
		//textLabel.SetTextString("TextString");
		//modelData.SetColor(defaultColor.buttonColor->r, defaultColor.buttonColor->g, defaultColor.buttonColor->b);
		textLabelData.textColor = defaultColor.textColor;
		currentState = STATE::NONE;
	}

	textLabelData.currentState = this->currentState;
	buttonWidth = textLabelData.textWidth + padding.left + padding.right;
	buttonHeight = textLabelData.textHeight + padding.top + padding.bottom;

	//if (!nameData.selected) {
	//	if (IsWithinObject(modelData, uiMousePos)) {
	//		//OnHover(modelData, nameData);
	//	}
	//	else {
	//		textLabel.SetTextString("TextString");
	//		currentState = STATE::NONE;
	//		modelData.SetColor(defaultColor.buttonColor.r, defaultColor.buttonColor.g, defaultColor.buttonColor.b);
	//	}
	//}
}

//void Button::DrawButton(Model& modelData, TextLabel& textLabelData) {
//	
//	switch (currentState) {
//	case(STATE::HOVERED):
//		// set model to button color
//		modelData.SetColor(hoveredColor.buttonColor->r, hoveredColor.buttonColor->g, hoveredColor.buttonColor->b);
//		break;
//	case(STATE::FOCUSED):
//		modelData.SetColor(focusedColor.buttonColor->r, focusedColor.buttonColor->g, focusedColor.buttonColor->b);
//		break;
//	default:
//		modelData.SetColor(defaultColor.buttonColor->r, defaultColor.buttonColor->g, defaultColor.buttonColor->b);
//		break;
//	}
//	
//	// draw button
//	//modelData.Draw(nullptr, nullptr);
//	//// draw label
//	//graphics.DrawLabel(textLabelData, textLabelData.relTransform, *textLabelData.textColor);
//}
//
//void Button::DrawButtonTex(Model& modelData, Tex& texData, TextLabel& textLabelData) {
//	// draw button
//	//modelData.Draw(&texData, nullptr);
//	//// draw label
//	//graphics.DrawLabel(textLabelData, textLabelData.relTransform, *textLabelData.textColor);
//}

//void Button::OnClick(Model& modelData, Name& nameData) {
//	//change color based Name->selected bool state;
//	currentState = STATE::SELECTED;
//	modelData.SetColor(focusedColor.buttonColor.r, focusedColor.buttonColor.g, focusedColor.buttonColor.b);
//
//	// may need set textLabel color - add to signature?
//}
//
//void Button::OnHover(Model& modelData, Name& nameData) {
//	textLabel.SetTextString("Hovered Text");
//	currentState = STATE::HOVERED;
//	modelData.SetColor(hoveredColor.buttonColor.r, hoveredColor.buttonColor.g, hoveredColor.buttonColor.b);
//}
//
//void Button::OnFocus() {
//	textLabel.SetTextString("Focused Text");
//	currentState = STATE::FOCUSED;
//	// open properties perhaps?
//}