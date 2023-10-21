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
*	@file		UIComponents.h
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
*	@date		18 October 2023
*
* *****************************************************************************
*
*	@brief		UI components used for in-game UI
*
*	UIComponents contianing UI components with their own niche properties
*	to be set and modified.
*
******************************************************************************/

#pragma once
#include "Components.h"
#include "model.h"
#include "Font.h"

// enums for alignment
enum class UI_HORIZONTAL_ALIGNMENT {
	H_LEFT_ALIGN,
	H_CENTER_ALIGN,
	H_RIGHT_ALIGN
};
enum class UI_VERTICAL_ALIGNMENT {
	V_CENTER_ALIGN,
	V_TOP_ALIGN,	
	V_BOTTOM_ALIGN
};
enum class UI_COLOR_STATES {
	DEFAULT,
	HOVERED,
	PRESSED
};

class UIComponent {
public:
	virtual ~UIComponent() {}
	
	//event handler functions
	virtual void onClick() = 0;
	virtual void onHover() = 0;
	virtual void onDrag() = 0;
	virtual void onFocus() = 0;
};

class Button : public UIComponent {
public:
	TextLabel textLabel;
	Vec2 padding{};

	// destination?

	void onClick() override {

	}

	void onHover() override {

	}

	void onDrag() override {

	}

	void onFocus() override {

	}
};

class TextLabel : public UIComponent {
public:
	Font* font{};
	std::string textString{};
	UI_HORIZONTAL_ALIGNMENT textAlignment{};

	TextLabel(Font& f, std::string str, UI_HORIZONTAL_ALIGNMENT align)
		: font(&f), textString(str), textAlignment(align) {}

	TextLabel(std::string str, UI_HORIZONTAL_ALIGNMENT align)
		: font(fonts.GetDefaultFont()), textString(str), textAlignment(align) {}

	void onClick() override {

	}

	void onHover() override {

	}

	void onDrag() override {

	}

	void onFocus() override {

	}
};

class LayoutGroup : UIComponent {
public:
	Vec2 margin{};
	//vector of component pointers? just to keep track what components are in the group?
	std::vector<std::shared_ptr<UIComponent>>components;
};



// UI manager class to handle global states (and event handling?)
