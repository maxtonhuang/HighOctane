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
#include "Colors.h"
#include "Events.h"

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
// enums for state lookup
enum class STATE {
	NONE,
	//SELECTED,
	HOVERED,
	FOCUSED
};

struct Padding {
	float left;
	float right;
	float top;
	float bottom;

	Padding() : left{ 40.f }, right{ 40.f }, top{ 20.f }, bottom{ 20.f } {}
	Padding(float horizontalVal, float verticalVal) : left{ horizontalVal }, right{ horizontalVal }, top{ verticalVal }, bottom{ verticalVal } {}
	Padding(float leftVal, float rightVal, float topVal, float bottomVal) : left{ leftVal }, right{ rightVal }, top{ topVal }, bottom{ bottomVal } {}
};

// struct for padding within padding component

class UIComponent {
public:
	virtual ~UIComponent() {}
	
	//event handler functions
	//virtual void Update(Transform& transformData, Model& modelData, Name& nameData) = 0;
	//virtual void OnClick(Model& modelData, Name& nameData) = 0;
	//virtual void OnHover(Model& modelData, Name& nameData) = 0;
	//virtual void OnFocus() = 0;
};

class TextLabel : public UIComponent {
public:
	Font* font{}; 
	std::string textString{}; 
	std::string prevTextString{};
	Vec2 posOffset{}; //offset from transform
	Vec2 relTransform{};
	glm::vec4 textColor{}; 
	float relFontSize{};
	float textWidth{};
	float textHeight{};
	UI_HORIZONTAL_ALIGNMENT textAlignment{};
	STATE currentState{};

	std::string initClr;

	// FUTURE IMPLEMENTATIONS
	// -> multiline, auto/fixed height
	// -> line height
	
	TextLabel();
	TextLabel(std::string str, std::string txtColor);
	TextLabel(std::string str, glm::vec4 clr);

	glm::vec4& GetTextColor();

	void SetTextString(std::string txtStr);
	void SetTextColor(glm::vec4 txtColor);
	void SetFontFamily(std::string newFamily);
	void SetFontVariant(std::string newFamily, std::string newVariant);

	bool CheckStringUpdated(TextLabel& txtLblData);	
	void CalculateOffset();
	void UpdateOffset(Transform const& transformData);

	void Update(Model& modelData, Name& nameData);
	/*void OnClick(Model& modelData, Name& nameData) override;
	void OnHover(Model& modelData, Name& nameData) override;
	void OnFocus() override;*/
};

class Button : public UIComponent {
public:
	struct ColorSet {
		glm::vec4 buttonColor; 
		glm::vec4 textColor; 
		glm::vec4 outlineColor;

		ColorSet() : buttonColor{ 1,1,1,1 }, textColor{ 0,0,0,1 }, outlineColor{ 1,1,1,1 } {}
		ColorSet(std::string btnColor, std::string txtColor) :
			buttonColor{ colors.colorMap[btnColor] }, textColor{ colors.colorMap[txtColor] }, outlineColor{ colors.colorMap[btnColor] } {}
		ColorSet(std::string btnColor, glm::vec4 txtColor) :
			buttonColor{ colors.colorMap[btnColor] }, textColor{ txtColor }, outlineColor{ colors.colorMap[btnColor] } {}
		ColorSet(glm::vec4 btnColor, std::string txtColor) :
			buttonColor{ btnColor }, textColor{ colors.colorMap[txtColor] }, outlineColor{ btnColor } {}
		ColorSet(glm::vec4 btnColor, glm::vec4 txtColor) :
			buttonColor{ btnColor }, textColor{ txtColor }, outlineColor{ btnColor } {}
	}colorSet;

	
	Vec2 posOffset{}; //offset from transform
	Vec2 relTransform{};
	std::string eventName{};
	std::string eventInput{};
	float buttonWidth{};
	float buttonHeight{};

	// other UI unique properties
	STATE currentState{};
	Padding padding{}; 

	// store colors for each state
	ColorSet defaultColor;
	ColorSet hoveredColor;
	ColorSet focusedColor;

	Button();
	Button(std::string btnColor, glm::vec4 txtColor);
	Button(glm::vec4 btnColor, glm::vec4 txtColor);

	glm::vec4& GetDefaultTextColor();
	glm::vec4& GetDefaultButtonColor();

	glm::vec4 GetButtonColor();

	void Update(Model& modelData, Name& nameData, TextLabel& textLabelData);
	void UpdateColorSets(glm::vec4 btnColor, glm::vec4 txtColor);

	/*void OnClick(Model& modelData, Name& nameData) override;
	void OnHover(Model& modelData, Name& nameData) override;
	void OnFocus() override;*/

	/*void DrawButton(Model& modelData, TextLabel& textLabelData);
	void DrawButtonTex(Model& modelData, Tex& texData, TextLabel& textLabelData);*/
};

class LayoutGroup : UIComponent {
public:
	Vec2 margin{};
	//vector of component pointers? just to keep track what components are in the group?
	std::vector<std::shared_ptr<UIComponent>>components;
};
