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
*	@date		[M2] 23 October 2023
*				[M3] 26 November 2023
*
* *****************************************************************************
*
*	@brief		UI components used for in-game UI
*
*	UIComponents contianing UI components with their own niche properties
*	to be set and modified.
* 
*	M2 -	core functionalities for TextLabel and Button
*	M3 -	refinements to TextLabel and Button (padding, alignment, resizing),
*			added all remaining UI required for battle scene (HealthBar,
			SkillPointHUD, AttackSkill, AllyHUD, EnemyHUD, Turn Indicator, 
			Status Effect and their	child components)
*
******************************************************************************/

#pragma once
#include "Components.h"
#include "CharacterStats.h"
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
	V_TOP_ALIGN,	
	V_CENTER_ALIGN,
	V_BOTTOM_ALIGN
};
enum class UI_TEXT_WRAP {
	AUTO_WIDTH,
	AUTO_HEIGHT,
	FIXED_SIZE
};
// enums for state lookup
enum class STATE {
	NONE,
	//SELECTED,
	HOVERED,
	FOCUSED,
	DISABLED
};

// struct for padding within padding component
struct Padding {
	float left;
	float right;
	float top;
	float bottom;
	int setting{1};

	Padding() : left{ 40.f }, right{ 40.f }, top{ 20.f }, bottom{ 20.f } {}
	Padding(float singleVal): left{ singleVal }, right{ singleVal }, top{ singleVal }, bottom{ singleVal } {}
	Padding(float horizontalVal, float verticalVal) : left{ horizontalVal }, right{ horizontalVal }, top{ verticalVal }, bottom{ verticalVal } {}
	Padding(float leftVal, float rightVal, float topVal, float bottomVal) : left{ leftVal }, right{ rightVal }, top{ topVal }, bottom{ bottomVal } {}
};

//struct for line by line text data
struct TextLine {
	std::string lineString{};
	float lineWidth{};
	Vec2 relTransform{};
};

class UIComponent {
public:
	virtual ~UIComponent() {}
};

class TextLabel : public UIComponent {
public:
	Font* font{}; 
	std::string textString{}; 
	std::string prevTextString{};
	//Vec2 posOffset{}; //offset from transform
	Vec2 relTransform{}; //phase out??
	glm::vec4 textColor{}; 
	float relFontSize{};
	float textWidth{};
	float textHeight{};
	float glyphHeight{};
	float lineHeight{};
	std::vector<TextLine> lineData{};
	//int numLines{};
	UI_TEXT_WRAP textWrap{};
	UI_HORIZONTAL_ALIGNMENT hAlignment{};
	UI_VERTICAL_ALIGNMENT vAlignment{};
	STATE currentState{};
	bool hasBackground{};

	std::string initClr;

	// FUTURE IMPLEMENTATIONS
	// -> multiline, auto/fixed height
	// -> line height

	/**************************
	******* CONSRUCTORS *******
	**************************/
	TextLabel();
	TextLabel(std::string str, std::string txtColor);
	TextLabel(std::string str, glm::vec4 clr);

	/**************************
	********* GETTERS *********
	**************************/
	glm::vec4& GetTextColor();

	/**************************
	********* SETTERS *********
	**************************/
	void SetTextString(std::string txtStr);
	void SetTextColor(glm::vec4 txtColor);
	void SetFontFamily(std::string newFamily);
	void SetFontVariant(std::string newFamily, std::string newVariant);

	/**************************
	******* OTHER UTILS *******
	**************************/
	bool CheckStringUpdated(TextLabel& txtLblData);
	void CalculateOffset();
	void UpdateOffset(Transform const& transformData, Size& sizeData, Padding const& paddingData = { 0.f,0.f,0.f,0.f });

	/**************************
	******* SYSTEM CALLS ******
	**************************/
	void Update(Model& modelData, Name& nameData);

	/**************************
	******* DEPRECATED ********
	**************************/
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

	int colorSetting{};
	bool isOverlay{};

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
	void UpdateColorSets(STATE currentState, glm::vec4 btnColor, glm::vec4 txtColor);
};

class HealthBar : UIComponent {
public:
	Entity entity{};
	CharacterStats* charaStatsRef{};
	float currentHealth{};
	float maxHealth{};	
	float healthPct{};

	bool showHealthStat{};
	bool showValOrPct{};

	float barWidth{};
	float barHeight{};

	HealthBar();
	void UpdateHealth();
	void UpdateTextDisplay(TextLabel& textLabelData);
};

class HealthRemaining : UIComponent {
public:
	// child entity of HealthBar
	float currentHealth{};

	void UpdateSize(HealthBar& parentHealthBar, Size& parentSize, Size& childSize);
	void UpdateColors(Model& childModel, CharacterStats& parentCharaStats);
	void UpdateOffset(Size& parentSize, HealthBar& parentHealthBar, Child& childData);

};


class SkillPointHUD : UIComponent {
public:
	// parent entity of SkillPoint (x5)
	// need to retrieve from battle system instead
	BattleSystem* battleSys{};
	int skillPointBalance{};
	int maxSkillPoints{};
	
	SkillPointHUD();
	void UpdateBalance();
};

class SkillPoint : UIComponent {
public:
	// child entity of SkillPointHUD
	bool isActive{};
};


//class AttackSkillsHUD : UIComponent {
//public:
//	//parent entity of AttackSkill (x3)
//	CharacterStats* charaStatsRef{};
//	std::vector<Attack> atkSkills{};
//};

class AttackSkill : UIComponent {
public:
	//parent entity of SkillIcon, SkillCost, AttackType
	//entity tagged with this component should have button component to implement button event
	int skillIndex{};
	//Attack* atkSkillRef{};

	//void UpdateSkillTex(Tex& texData);
	void UpdateSkillEvent(Button& buttonData);
	void UpdateButtonState(Button& buttonData, bool isSufficient);
	void UpdateAtkTypeLbl(TextLabel& textLabelData, AttackType atkType);
	//void UpdateAtkTypeIcon(Tex& texData, AttackType atktype);
	void UpdateSkillCostLbl(TextLabel& textLabelData, int skillCost);
};

class SkillIcon : UIComponent {
	// empty by design - tags skill's texture entity
};

class SkillCost : UIComponent {
	// empty by design - tags skill's chi cost label
};

class SkillAttackType : UIComponent {
	// TextLabel + Tex
	// empty by design - tags skill's atk type lbl (TextLabel) and icon (Tex)
};

class AllyHUD : UIComponent {
public:
	int allyIndex{};

	void CheckValidIndex(int playerCount, bool& res);

	// FUTURE IMPLEMENTATIONS: dynamic allocation!!
	//static int allyInstanceIndex;
	//AllyHUD() : allyIndex{ allyInstanceIndex++ } {}
};

class EnemyHUD : UIComponent {
public:
	int enemyIndex{};
	Entity statuslabel{};
	bool initialised{ false };

	void CheckValidIndex(int enemyCount, bool& res);
	void ToggleStatusFx(Entity parentEntity, int stacks);
};

class TurnIndicator : UIComponent {
public:
	Entity character{}; //character this indicator links to
};


//class StatusEffectsPanel : UIComponent {
//public:
//	UI_HORIZONTAL_ALIGNMENT hAlignment{};
//
//	/*
//	* notes:
//	* - for future implementation where more status effects are introduced! kiv!!
//	* - parent entity to hold multiple "Effect" (prefab?)
//	* - not meant to be modifiable, to constantly get info from CharacterStats and display accordingly
//	* - usage: left aligned in Ally HUD, right aligned in enemy HUD
//	*/
//
//	//void UpdateOffset();
//};

class StatusEffect : UIComponent {
public:
	/*
	* notes:
	* - composite of icon (Tex) and stacks (TextLabel)
	*/
	Entity character{};
	void UpdateOffset(Size& parentSize, Transform& parentTransform, Transform& childTransform);
	void UpdateStacksLbl(TextLabel& textLabelData, int stacks);
};