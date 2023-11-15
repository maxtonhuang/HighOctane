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

/*!
* \brief TextLabel default constructor
*
* Initializes TextLabel component, with no parameters as specifiers.
*
*/
TextLabel::TextLabel() {
	font = fonts.GetDefaultFont();
	textString = "TextLabel";
	hAlignment = UI_HORIZONTAL_ALIGNMENT::H_CENTER_ALIGN;
	vAlignment = UI_VERTICAL_ALIGNMENT::V_CENTER_ALIGN;
	relFontSize = 0.5f;
	textColor = colors.colorMap["black"];
}

/*!
* \brief TextLabel default constructor
*
* Initializes TextLabel component, with text and color strings provided.
*
*/
TextLabel::TextLabel(std::string str, std::string txtColor) {
	font = fonts.GetDefaultFont();
	textString = str;
	hAlignment = UI_HORIZONTAL_ALIGNMENT::H_CENTER_ALIGN;
	vAlignment = UI_VERTICAL_ALIGNMENT::V_CENTER_ALIGN;
	relFontSize = 0.5f;
	textColor = colors.colorMap[txtColor];
	initClr = txtColor;
}

/*!
* \brief TextLabel default constructor
*
* Initializes TextLabel component, with text string and glm::vec4 color provided.
*
*/
TextLabel::TextLabel(std::string str, glm::vec4 clr) {
	font = fonts.GetDefaultFont();
	textString = str;
	hAlignment = UI_HORIZONTAL_ALIGNMENT::H_CENTER_ALIGN;
	vAlignment = UI_VERTICAL_ALIGNMENT::V_CENTER_ALIGN;
	relFontSize = 0.5f;
	textColor = clr;
}

/*!
* \brief textColor getter
*
* Retrieves color set for TextLabel
*
*/
glm::vec4& TextLabel::GetTextColor() {
	return textColor;
}

/*!
* \brief textString setter
*
* Retrieves color set for TextLabel
*
*/
void TextLabel::SetTextString(std::string txtStr) {
	textString = txtStr;
}

/*!
* \brief textColor setter
*
* Sets color for TextLabel
*
*/
void TextLabel::SetTextColor(glm::vec4 txtColor) {
	textColor = txtColor;
}

/*!
* \brief font setter
*
* Usage: imgui font selector. variant is set to Regular on default to prevent passing
* variant name of old font, which new font may not have, when calling GetFont()
*
*/
void TextLabel::SetFontFamily(std::string newFamily) {
	Font* newFont = fonts.GetFont(newFamily, "Regular");
	if (!newFont) {
		newFont = fonts.GetFont(newFamily, fonts.GetFontVariantList(newFamily).front());
	}
	if (!newFont) {
		newFont = fonts.GetDefaultFont();
	}
	font = newFont;
}

/*!
* \brief font setter
*
* Usage: imgui font selector. Retrieves requested font when calling GetFont()
*
*/
void TextLabel::SetFontVariant(std::string newFamily, std::string newVariant) {
	Font* newFont = fonts.GetFont(newFamily, newVariant);
	if (!newFont) {
		newFont = fonts.GetDefaultFont();
	}
	font = newFont;
}

/*!
* \brief text string boolean checker
*
* Usage: imgui font selector. Retrieves requested font when calling GetFont()
*
*/
bool TextLabel::CheckStringUpdated(TextLabel& txtLblData) {
	if (txtLblData.textString != txtLblData.prevTextString) {
		prevTextString = txtLblData.textString;
		return true;
	}
	return false;
}

/*!
* \brief calculate offset
*
* Usage: called after UITextLabelSystem Upate(). calculates min width and height
* required for drawing text string.
*
*/
void TextLabel::CalculateOffset() {
	//DEBUG_PRINT("Recalculating...");
	// reset variables
	textWidth = 0.f;
	textHeight = 0.f;
	float verticalPadding = static_cast<float>(-(*font).largestNegativeOffset);

	std::string::const_iterator c;
	for (c = textString.begin(); c != textString.end(); c++)
	{
		Character ch{ (*font).characters[*c] };

		float w = ch.size.x * relFontSize;
		float h = ch.size.y * relFontSize;

		// calculate size needed
		if (*c != textString[textString.size()]) {
			textWidth += ((ch.advance >> 6) * relFontSize) - w;
		}
		textWidth += (w + (ch.bearing.x * relFontSize));
		textHeight = std::max(textHeight, h);
	}

	//finalise y-offset (font data)
	textHeight += verticalPadding;
}

/*!
* \brief rel transform setter
*
* Internal function. currently re-centers textString's glyphs to middle of entity model
* FUTURE IMPLEMENTATIONS: will likely change when alignment is implemented.
*
*/
void TextLabel::UpdateOffset(Transform const& transformData, Size const& sizeData, Padding const& paddingData) {
	CalculateOffset();

	switch (hAlignment) {
	case(UI_HORIZONTAL_ALIGNMENT::H_LEFT_ALIGN):
		//left align
		relTransform.x = transformData.position.x - (0.5f * sizeData.width) + paddingData.left;
		break;
	case(UI_HORIZONTAL_ALIGNMENT::H_RIGHT_ALIGN):
		//right align
		relTransform.x = transformData.position.x - textWidth + (0.5f * sizeData.width) - paddingData.right;
		break;
	default:
		//center align
		relTransform.x = transformData.position.x - (0.5f * textWidth);
		break;
	}
	
	switch (vAlignment) {
	case(UI_VERTICAL_ALIGNMENT::V_TOP_ALIGN):
		//top align
		relTransform.y = transformData.position.y + (0.5f * sizeData.height - textHeight) - paddingData.top;
		break;
	case(UI_VERTICAL_ALIGNMENT::V_BOTTOM_ALIGN):
		//bottom align
		relTransform.y = transformData.position.y - textHeight - (0.5f * sizeData.height - textHeight) + paddingData.bottom;
		break;
	default:
		//center align
		relTransform.y = transformData.position.y - (0.25f * textHeight);
		break;
	}
}

/*!
* \brief TextLabel Update()
*
* Called in UITextLabelSystem's update. Updates currentState accordingly.
*
*/
void TextLabel::Update(Model& modelData, Name& nameData) {
	// get cursorPos, compare with pos in Transform, return if no match
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::UICOMPONENT]) {
		switch (msg.type) {
		case(TYPE::MOUSE_MOVE):
			uiMousePos = { msg.posX, msg.posY };
			break;
		}		
	}

	if (nameData.selected && edit_mode) {
		currentState = STATE::FOCUSED;
	}
	else if (IsWithinObject(modelData, uiMousePos)) {
		currentState = STATE::HOVERED;
	}
	else {
		currentState = STATE::NONE;
	}
}

/**************************
********* BUTTON **********
**************************/
/*!
* \brief Button default constructor
*
* Initializes Button component, with no parameters as specifiers.
*
*/
Button::Button() {
	std::string txtStr = "TextString";
	std::string btnColor = "white";
	std::string txtColor = "blue";
	defaultColor = { btnColor, txtColor };
	hoveredColor = { txtColor, btnColor };
	focusedColor = { txtColor, btnColor };
}

/*!
* \brief Button default constructor
*
* Initializes Button component, with button color string and text color values provided.
*
*/
Button::Button(std::string btnColor, glm::vec4 txtColor) {
	std::string txtStr = "TextString";
	defaultColor = { btnColor, txtColor };
	hoveredColor = { txtColor, btnColor };
	focusedColor = { txtColor, btnColor };
}

/*!
* \brief Button default constructor
*
* Initializes Button component, with button and text color values provided.
*
*/
Button::Button(glm::vec4 btnColor, glm::vec4 txtColor) {
	std::string txtStr = "TextString";
	defaultColor = { btnColor, txtColor };
	hoveredColor = { txtColor, btnColor };
	focusedColor = { txtColor, btnColor };
}

/*!
* \brief Button default color getter
*
* Retrieves default text color currently set.
*
*/
glm::vec4& Button::GetDefaultTextColor() {
	return defaultColor.textColor;
}

/*!
* \brief Button default color getter
*
* Retrieves default button color currently set.
*
*/
glm::vec4& Button::GetDefaultButtonColor() {
	return defaultColor.buttonColor;
}


/*!
* \brief Button color getter
*
* Retrieves button color based on current state. Used for determining what color to pass
* on to the Model component when updating the button's color.
*
*/
glm::vec4 Button::GetButtonColor() {
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

/*!
* \brief Button Update()
*
* Called in UIButtonSystem's update. Updates currentState accordingly.
*
*/
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
				}
			}
			break;
		}
	}

	// update button and text colors
	if (nameData.selected && edit_mode) {
		currentState = STATE::FOCUSED;
	}
	else if (IsWithinObject(modelData, uiMousePos)) {
		currentState = STATE::HOVERED;
	}
	else {
		currentState = STATE::NONE;
	}

	//outside edit mode, color change accordingly to state. otherwise show default
	if (!edit_mode) {
		switch (currentState) {
		case(STATE::HOVERED):
			textLabelData.textColor = hoveredColor.textColor;
			break;
		case(STATE::FOCUSED):
			textLabelData.textColor = focusedColor.textColor;
			break;
		default:
			textLabelData.textColor = defaultColor.textColor;
			break;
		}
	}
	//else {
	//	//UpdateColorSets(GetDefaultButtonColor(), GetDefaultTextColor());
	//	textLabelData.textColor = defaultColor.textColor;
	//}


	// note: padding should NOT affect overall size of button, since it acts within the button confines
	textLabelData.currentState = this->currentState;
	buttonWidth = textLabelData.textWidth /*+ padding.left + padding.right*/;
	buttonHeight = textLabelData.textHeight /*+ padding.top + padding.bottom*/;
}

/*!
* \brief Button Update ColorSets
*
* Outside of editor mode, constantly ensures hovered ColorSet and focused ColorSet are
* up to date based on default ColorSet
*
*/
void Button::UpdateColorSets(glm::vec4 btnColor, glm::vec4 txtColor) {
	defaultColor = { btnColor, txtColor };
	hoveredColor = { txtColor, btnColor };
	focusedColor = { txtColor, btnColor };
}


/**************************
********* ARCHIVED ********
**************************/

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
// 
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