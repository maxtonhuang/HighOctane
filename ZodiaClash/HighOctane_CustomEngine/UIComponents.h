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
//enum class UI_COLOR_STATES {
//	DEFAULT,
//	HOVERED,
//	PRESSED
//};

class UIComponent {
public:
	virtual ~UIComponent() {}
	
	//event handler functions
	virtual void IsClickedOrHovered(Transform& transformData, Model& modelData, Name& nameData) = 0;
	virtual void OnClick(Model& modelData, Name& nameData) = 0;
	virtual void OnHover(Model& modelData, Name& nameData) = 0;
	virtual void OnFocus() = 0;
};

class TextLabel : public UIComponent {
public:
	Font* font{};
	std::string textString{};
	std::string prevTextString{};
	UI_HORIZONTAL_ALIGNMENT textAlignment{};
	float relFontSize{};
	Vec2 posOffset{}; //offset from transform
	Vec2 relTransform{};

	// store colors for each state? -- default
	glm::vec4 defaultColor{};
	glm::vec4 hoveredColor{};
	glm::vec4 focusedColor{};


	TextLabel();
	TextLabel(Font& f, std::string str, UI_HORIZONTAL_ALIGNMENT align);
	TextLabel(std::string str, UI_HORIZONTAL_ALIGNMENT align);

	bool CheckStringUpdated(TextLabel& txtLblData);
	void SetTextString(std::string txtStr);
	void CalculateOffset();
	void UpdateOffset(Vec2& relTransform, Transform const& transformData);

	void IsClickedOrHovered(Transform& transformData, Model& modelData, Name& nameData) override;
	void OnClick(Model& modelData, Name& nameData) override;
	void OnHover(Model& modelData, Name& nameData) override;
	void OnFocus() override;
};

class Button : public UIComponent {
public:
	TextLabel textLabel;
	Vec2 padding{};

	void IsClickedOrHovered(Transform& transformData, Model& modelData, Name& nameData) override;
	void OnClick(Model& modelData, Name& nameData) override;
	void OnHover(Model& modelData, Name& nameData) override;
	void OnFocus() override;
};

class LayoutGroup : UIComponent {
public:
	Vec2 margin{};
	//vector of component pointers? just to keep track what components are in the group?
	std::vector<std::shared_ptr<UIComponent>>components;
};
