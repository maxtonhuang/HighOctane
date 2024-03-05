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

#include "UIComponents.h"
#include "message.h"
#include "Global.h"
#include "Utilities.h"
#include "Font.h"
#include "graphics.h"
#include "Colors.h"
#include "AssetManager.h"
#include "Layering.h"
#include "Animation.h"

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
	textWrap = UI_TEXT_WRAP::FIXED_SIZE;
	relFontSize = 0.5f;
	textColor = colors.colorMap["black"];
	lineHeight = 1.5f;
}

/*!
* \brief TextLabel constructor
*
* Initializes TextLabel component, with text and color strings provided.
*
*/
TextLabel::TextLabel(std::string str, std::string txtColor) {
	font = fonts.GetDefaultFont();
	textString = str;
	hAlignment = UI_HORIZONTAL_ALIGNMENT::H_CENTER_ALIGN;
	vAlignment = UI_VERTICAL_ALIGNMENT::V_CENTER_ALIGN;
	textWrap = UI_TEXT_WRAP::FIXED_SIZE;
	relFontSize = 0.5f;
	textColor = colors.colorMap[txtColor];
	initClr = txtColor;
	lineHeight = 1.5f;
}

/*!
* \brief TextLabel constructor
*
* Initializes TextLabel component, with text string and glm::vec4 color provided.
*
*/
TextLabel::TextLabel(std::string str, glm::vec4 clr) {
	font = fonts.GetDefaultFont();
	textString = str;
	hAlignment = UI_HORIZONTAL_ALIGNMENT::H_CENTER_ALIGN;
	vAlignment = UI_VERTICAL_ALIGNMENT::V_CENTER_ALIGN;
	textWrap = UI_TEXT_WRAP::FIXED_SIZE;
	relFontSize = 0.5f;
	textColor = clr;
	lineHeight = 1.5f;
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
	glyphHeight = 0.f;
	float verticalPadding = static_cast<float>(-(*font).largestNegativeOffset);

	std::string::const_iterator c;
	std::stringstream textStream{ textString };
	std::string tmpWord{};
	std::string tmpLine{};
	TextLine newline{};
	float longestLineWidth{ 0.f };

	lineData.clear();

	while (std::getline(textStream,tmpLine)) {
		std::stringstream lineStream{ tmpLine };
		//newline.lineWidth = verticalPadding;
		while (lineStream >> tmpWord) {
			float wordWidth = 0.f;
			float glyphSpace = 0.f;
			if (!lineStream.eof()) {
				tmpWord += " ";
			}
			for (c = tmpWord.begin(); c != tmpWord.end(); c++) {
				Character ch{ (*font).characters[*c] };
				float w = ch.size.x * relFontSize;
				float h = ch.size.y * relFontSize;

				// calculate size needed
				if (*c != tmpWord[tmpWord.size()]) {
					glyphSpace = ((ch.advance >> 6) * relFontSize) - w;
					wordWidth += glyphSpace;
				}
				wordWidth += (w + (ch.bearing.x * relFontSize));

				// ignore space if exceeds width
				if ((*c == ' ') && (textWrap != UI_TEXT_WRAP::AUTO_WIDTH) && (newline.lineWidth + wordWidth + verticalPadding > textWidth)) {
					wordWidth -= glyphSpace;
				}
				glyphHeight = std::max(glyphHeight, h);
			}
			if ((textWrap != UI_TEXT_WRAP::AUTO_WIDTH) && (newline.lineWidth + wordWidth + verticalPadding > textWidth)) {
				//push newline
				lineData.push_back(newline);
				newline = {};
			}
			newline.lineString += tmpWord;
			newline.lineWidth += wordWidth;
			if (longestLineWidth < newline.lineWidth) {
				longestLineWidth = newline.lineWidth;
			}
		}
		lineData.push_back(newline);
		newline = {};
	}

	//update textWidth, textHeight
	switch (textWrap) {
	case UI_TEXT_WRAP::AUTO_WIDTH:
		textWidth = longestLineWidth + verticalPadding;
		textHeight = glyphHeight * lineHeight * lineData.size();
		break;
	case UI_TEXT_WRAP::AUTO_HEIGHT:
		textHeight = glyphHeight * lineHeight * lineData.size();
		break;
	default:
		textHeight = glyphHeight * lineHeight * lineData.size();
		break;
	}
	textHeight += verticalPadding;
}

/*!
* \brief rel transform setter
*
* Internal function. 
* [M3] currently re-centers textString's glyphs to middle of entity model
* [M4] now aligns textString based on hAlignment and vAlignment, resizes entity based on textWrap.
*		updates starting point for text to start drawing based off calculated dimensions.
*
*/
void TextLabel::UpdateOffset(Transform const& transformData, Size& sizeData, Padding const& paddingData) {	

	CalculateOffset();

	switch (textWrap) {
	case(UI_TEXT_WRAP::AUTO_WIDTH):
		//snaps label width to calculated width, label height remains as calculated height
		sizeData.width = textWidth + paddingData.left + paddingData.right;
		sizeData.height = std::max(sizeData.height, textHeight);
		break; 
	case(UI_TEXT_WRAP::AUTO_HEIGHT):
		//snaps label height to calculated height (label width remains adjustable)
		textWidth = sizeData.width;
		sizeData.height = textHeight + paddingData.top + paddingData.bottom;
		break;
	default:
		textWrap = UI_TEXT_WRAP::FIXED_SIZE;
		//initial state before text wrap was implemented
		textWidth = sizeData.width;
		textHeight = sizeData.height;
		break;
	}

	int lineCount{};
	float verticalPadding = static_cast<float>(-(*font).largestNegativeOffset);
	for (TextLine& line : lineData) {
		// calculate x-offset
		switch (hAlignment) {
		case(UI_HORIZONTAL_ALIGNMENT::H_LEFT_ALIGN):
			//left align
			line.relTransform.x = transformData.position.x - (0.5f * sizeData.width) + paddingData.left + (0.5f * verticalPadding);
			break;
		case(UI_HORIZONTAL_ALIGNMENT::H_RIGHT_ALIGN):
			//right align
			line.relTransform.x = transformData.position.x - (line.lineWidth - (0.5f * sizeData.width)) - paddingData.right - (0.5f * verticalPadding);
			break;
		default:
			//center align
			line.relTransform.x = transformData.position.x - (0.5f * line.lineWidth);		
			break;
		}

		// calculate y-offset
		switch (vAlignment) {
		case(UI_VERTICAL_ALIGNMENT::V_TOP_ALIGN):
			//top align
			line.relTransform.y = transformData.position.y + (0.5f * sizeData.height - glyphHeight) - (0.5f * verticalPadding) - paddingData.top;
			break;
		case(UI_VERTICAL_ALIGNMENT::V_BOTTOM_ALIGN):
			//bottom align
			line.relTransform.y = transformData.position.y - textHeight - (0.5f * sizeData.height - textHeight) + (0.5f * verticalPadding) + paddingData.bottom;
			line.relTransform.y += ((lineData.size() - 1) * glyphHeight * lineHeight);
			
			break;
		default:
			//center align
			line.relTransform.y = transformData.position.y - (0.5f * glyphHeight);
			line.relTransform.y += (0.5f * (lineData.size() - 1) * glyphHeight * lineHeight);
			break;
		}
		
		line.relTransform.y -= (lineCount * glyphHeight * lineHeight);
		lineCount++;
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

	if (nameData.selected && GetCurrentSystemMode() == SystemMode::EDIT) {
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
* \brief Button constructor
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
* \brief Button constructor
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
	case(STATE::DISABLED):
		return colors.colorMap["secondary"];
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
void Button::Update(Model& modelData, Name& nameData, TextLabel& textLabelData, Entity entity) {
	static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
	// get cursorPos, compare with pos in Transform, return if no match
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::UICOMPONENT]) {
		switch (msg.type) {
		case(TYPE::MOUSE_MOVE):
			uiMousePos = { msg.posX, msg.posY };
			break;
		case(TYPE::MOUSE_CLICK):
			if (IsWithinObject(modelData, uiMousePos)) {
				//on click event trigger (outside edit mode)
				if ((GetCurrentSystemMode() == SystemMode::RUN || GetCurrentSystemMode() == SystemMode::PAUSE || GetCurrentSystemMode() == SystemMode::GAMEHELP || GetCurrentSystemMode() == SystemMode::EXITCONFIRM) && (currentState != STATE::DISABLED) && !eventName.empty()) {
					
					if (GetCurrentSystemMode() == SystemMode::GAMEHELP || GetCurrentSystemMode() == SystemMode::EXITCONFIRM) {
						if (nameData.name.find("pause") == 0) {
							return;
						}
					}

					if (GetCurrentSystemMode() == SystemMode::EXITCONFIRM) {
						if (nameData.name == "startButton" || nameData.name == "quitButton") {
							return;
						}
					}
					if (animationArray.HasComponent(entity)) {
						animationArray.GetData(entity).Queue("Click", entity);
					}
					events.Call(eventName, eventInput);
				}
			}
			break;
		}
	}

	// update button and text colors
	if (nameData.selected && GetCurrentSystemMode() == SystemMode::EDIT) {
		currentState = STATE::FOCUSED;
	}

	if (currentState != STATE::DISABLED) {
		if (IsWithinObject(modelData, uiMousePos)) {
			currentState = STATE::HOVERED;
		}
		else {
			currentState = STATE::NONE;
		}
	}

	//outside edit mode, color change accordingly to state. otherwise show default
	if (GetCurrentSystemMode() == SystemMode::RUN || GetCurrentSystemMode() == SystemMode::PAUSE) {
		switch (currentState) {
		case(STATE::HOVERED):
			if (previousState == STATE::NONE) {
				if (animationArray.HasComponent(entity)) {
					animationArray.GetData(entity).Queue("Hover", entity);
				}
				else {
					textLabelData.textColor = hoveredColor.textColor;
				}
			}
			previousState = STATE::HOVERED;
			break;
		case(STATE::FOCUSED):
			textLabelData.textColor = focusedColor.textColor;
			break;
		case(STATE::DISABLED):
			textLabelData.textColor = colors.colorMap["secondary"];
			break;
		case(STATE::NONE):
			if (previousState == STATE::HOVERED) {
				if (animationArray.HasComponent(entity)) {
					animationArray.GetData(entity).Queue("Reset", entity);
				}
				else {
					textLabelData.textColor = hoveredColor.textColor;
				}
			}
			previousState = STATE::NONE;
			break;
		default:
			textLabelData.textColor = defaultColor.textColor;
			break;
		}
	}

	// current implementation:  to ignore padding for fixed size buttons unless sufficient space (act within button confines)
	// future implementation to follow figma's auto layout "hug contents" setting (requires 2 separate settings)
	// NOTE: padding only applies for the specified dimension to auto!
	textLabelData.currentState = this->currentState;
	buttonWidth = textLabelData.textWidth;
	buttonHeight = textLabelData.textHeight;
}

/*!
* \brief Button Update ColorSets
*
* For color fine tuning in imgui
*
*/
void Button::UpdateColorSets(STATE currState, glm::vec4 btnColor, glm::vec4 txtColor) {
	switch (currState) {
	case(STATE::HOVERED):
		hoveredColor = { btnColor, txtColor };
		break;
	case(STATE::FOCUSED):
		focusedColor = { btnColor, txtColor };
		break;
	default:
		defaultColor = { btnColor, txtColor };
		break;
	}
}


/**************************
********* HP BAR **********
**************************/

/*!
* \brief HealthBar default constructor
*
* Sets initial display toggles on intialization
*
*/
HealthBar::HealthBar() {
	showHealthStat = true;
	showValOrPct = true;
}

/*!
* \brief HealthBar Update health values
*
* Updates HP bar based on tagged characterStats.
* Else remains constant (bar will be white)
*
*/
void HealthBar::UpdateHealth() {
	//read data from character stats
	if (charaStatsRef) {
		maxHealth = charaStatsRef->stats.maxHealth;
		currentHealth = charaStatsRef->stats.health;
	}
	else {
		maxHealth = 1000.f;
		currentHealth = maxHealth;
	}
	healthPct = (currentHealth / maxHealth) * 100.f;
}

/*!
* \brief HealthBar Update text label
*
* Updates text label based on showValOrPct setting
* If true - shows rounded integral value
* Else - shows health percentage at 1d.p.
*
*/
void HealthBar::UpdateTextDisplay(TextLabel& textLabelData) {
	if (showHealthStat) {
		std::ostringstream oss;
		if (!showValOrPct) {
			//show val
			oss << std::fixed << std::setprecision(0) << currentHealth;
		}
		else {
			//show percentage
			oss << std::fixed << std::setprecision(1) << healthPct << "%";
		}
		textLabelData.textString = oss.str();
	}
	else {
		textLabelData.textString = "";
	}
}

/*!
* \brief HealthRemaining Update health bar length
*
* Calculates health bar dimensions with reference to parent health bar dimensions and healthPct
*
*/
void HealthRemaining::UpdateSize(HealthBar& parentHealthBar, Size& parentSize, Size& childSize) {
	childSize.width = parentSize.width * (parentHealthBar.healthPct / 100.f) * 0.95f;
	childSize.height = parentSize.height * 0.8f; // to make customizable?
}

/*!
* \brief HealthRemaining Update color
*
* Sets HP bar color based tagged CharacterStats ref
* Case 1 - no CharacterStats ref (battle system is !active/win/lose), white bar
* Case 2 - player, green bar
* Case 3 - enemy, red bar
*
*/
void HealthRemaining::UpdateColors(Model& modelData, CharacterStats& parentCharaStats) {
	if (!(&parentCharaStats)) {
		modelData.SetColor(1.f, 1.f, 1.f);
		return;
	}

	if (parentCharaStats.tag == CharacterType::PLAYER) {
		modelData.SetColor(0.f,1.f,0.f);
	}
	else if (parentCharaStats.tag == CharacterType::ENEMY) {
		modelData.SetColor(1.f, 0.f, 0.f);
	}
	//else {
	//	modelData.SetColor(1.f, 1.f, 1.f);
	//}
}

/*!
* \brief HealthRemaining Update offset
*
* Offsets currentHP bar based off parentHP bar and healthPct
*
*/
void HealthRemaining::UpdateOffset(Size& parentSize, HealthBar& parentHealthBar, Child& childData) {
	childData.offset.position.x = (-0.5f * parentSize.width) + (parentHealthBar.healthPct / 100.f * 0.5f * parentSize.width);
}


/**************************
***** SKILLPT SYSTEM ******
**************************/

/*!
* \brief SkillPointHUD default constructor
*
* Initializes the SkillPoint system component. 
* Reads max value from battle system, otherwise is set to 0 for identification
*
*/
SkillPointHUD::SkillPointHUD() {
	battleSys = events.GetBattleSystem();
	maxSkillPoints = (battleSys) ? battleSys->chi : 0;
	skillPointBalance = maxSkillPoints;
}

/*!
* \brief SkillPointHUD default constructor
*
* Reads current balance from battle system
* This will only be called if:
* - battle system is active
* - battle state is player's turn + activeCharacter characterType is PLAYER
*
*/
void SkillPointHUD::UpdateBalance() {
	battleSys = events.GetBattleSystem();
	skillPointBalance = battleSys->chi;
}


/**************************
**** ATK SKILL SYSTEM *****
**************************/

//void AttackSkill::UpdateSkillTex(Tex& texData) {
//	// FUTURE IMPLEMENTATION: retrieve skill tex (to be stored)
//}

/*!
* \brief AttackSkill update button event
*
* Reinforces atkSkill button to always call the Select Skill event.
* Entity requirements: Parent, AttackSkill and Button (system signature)
*
*/
void AttackSkill::UpdateSkillEvent(Button& buttonData) {
	buttonData.eventName = "Select Skill";
	buttonData.eventInput = std::to_string(skillIndex+ 1);
}

/*!
* \brief AttackSkill update button state
*
* Updates button based on skill use eligibility
* (if there's sufficient chi for activation)
* Entity requirements: Parent, AttackSkill and Button (system signature)
*
*/
void AttackSkill::UpdateButtonState(Button& buttonData, bool isSufficient) {
	if (!isSufficient) {
		buttonData.currentState = STATE::DISABLED;
	}
	else {
		buttonData.currentState = STATE::NONE;
	}
}

/*!
* \brief AttackSkill update attack type label
*
* Updates button label to indicate attack type
* Entity requirements: Child, SkillAttackType and TextLabel
* 
* FUTURE CONSIDERINGS: 
* - allow for customization of attack type? 
*	will have to involve serialization
*
*/
void AttackSkill::UpdateAtkTypeLbl(TextLabel& textLabelData, AttackType atkType) {
	if (atkType == AttackType::AOE) {
		textLabelData.textString = "AOE";
		textLabelData.hAlignment = UI_HORIZONTAL_ALIGNMENT::H_CENTER_ALIGN;
	}
	else {
		textLabelData.textString = "Single Target";
	}
	//textLabelData.textString = (atkType == AttackType::NORMAL) ? "Single Target" : "AOE";
}

// FUTURE IMPLEMENTATION: to display attack type icon accordingly
//void AttackSkill::UpdateAtkTypeIcon(Tex& texData, AttackType atktype) {
//	//texData.tex = (atkType == AttackType::NORMAL) ? /* get asset: ST icon */ : /* get asset: AOE icon */;
//}

/*!
* \brief AttackSkill update skill cost label
*
* Updates button label to show skill's chi cost
* Entity requirements: Child, SkillCost and TextLabel
*
*/
void AttackSkill::UpdateSkillCostLbl(TextLabel& textLabelData, int skillCost) {
	textLabelData.textString = (skillCost > 0) ? " -" : " +";
	textLabelData.textString += std::to_string(std::abs(skillCost));
}


/**************************
***** ALLY HUD SYSTEM *****
**************************/

/*!
* \brief AllyHUD check valid index
*
* Reinforces referencing index set to always be valid.
* Returns false if battle system is not active
*
*/
void AllyHUD::CheckValidIndex(int playerCount, bool& result) {
	if (!playerCount) {
		result = false;
		return;
	}
	result = true;
	allyIndex = allyIndex % playerCount;
}


/**************************
**** ENEMY HUD SYSTEM *****
**************************/

/*!
* \brief EnemyHUD check valid index
*
* Reinforces referencing index set to always be valid.
* Returns false if battle system is not active
*
*/
void EnemyHUD::CheckValidIndex(int enemyCount, bool& result) {
	if (!enemyCount) {
		result = false;
		return;
	}
	result = true;
	enemyIndex = enemyIndex % enemyCount;
}

/*!
* \brief EnemyHUD toggle status effect indicator
*
* Event based trigger where it appears if enemy has debuff stacks
*
*/
void EnemyHUD::ToggleStatusFx(Entity parent, int stacks) {
	//DEBUG_PRINT("STACKS: %d", stacks);
	if ((statuslabel != 0) && (stacks < 1)) {
		EntityFactory::entityFactory().DeleteCloneModel(statuslabel);
		statuslabel = 0;
	}
	else {
		if ((statuslabel == 0) && (stacks > 0)) {
			statuslabel = EntityFactory::entityFactory().ClonePrefab("statusEffect.prefab");
			if (ECS::ecs().HasComponent<StatusEffect>(statuslabel)) {
				StatusEffect& statusFxComp{ ECS::ecs().GetComponent<StatusEffect>(statuslabel) };
				statusFxComp.character = parent;
			}
		}
	}
}


/************************************
**** ENEMY STATUS EFFECT SYSTEM *****
************************************/

/*!
* \brief StatusEffect update offset
*
* Calculates position for indicator based off parent entity position
*
*/
void StatusEffect::UpdateOffset(Size& parentSize, Transform& parentTransform, Transform& childTransform) {
	childTransform.position.y = parentTransform.position.y + (0.5f * parentSize.height);
	childTransform.position.x = parentTransform.position.x - (0.8f * parentSize.width);
}

/*!
* \brief StatusEffect update stacks label
*
* Updates number of stacks based on value read from battle system
* (CharacterStats tagged to parent entity)
*
*/
void StatusEffect::UpdateStacksLbl(TextLabel& textLabelData, int stacks) {
	textLabelData.textString = std::to_string(stacks);
}


/*************************
**** DIALOGUE SYSTEM *****
**************************/

bool DialogueHUD::DialoguePtrComparator::operator() (const Dialogue* d1, const Dialogue* d2) const {
	// Compare the triggers based on their priority
	if (d1->triggerType != d2->triggerType) {
		// priority is based on the enum order
		return static_cast<int>(d1->triggerType) > static_cast<int>(d2->triggerType);
	}

	// If all attributes are equal, return false (indicating equal priority)
	return false;
}

DialogueHUD::DialogueHUD() {
	dialogues.reserve(10);
}

void DialogueHUD::Initialize() {
	// if queue empty, populate queue
	if (dialogueQueue.empty()) {
		for (Dialogue dialogue : dialogues) {
			if (!dialogue.isTriggered)
				dialogueQueue.push(&dialogue);
		}
	}
};

/*!
* \brief DialogueHUD StartDialogue
*
* Triggers transition for dialogue UI to move in, sets isActive to true
*
*/
void DialogueHUD::StartDialogue(Entity entity, DIALOGUE_TRIGGER inputTriggerType) {
	// check if currentDialogue assigned by system does not match inputTriggerType
	BattleSystem* battleSys = events.GetBattleSystem();
	if (currentDialogue && (currentDialogue->triggerType != inputTriggerType)) {
		dialogueQueue.push(currentDialogue);
		currentDialogue = nullptr;
	}

	// check if dialogue at top of queue is same as inputTriggerType
	std::vector<Dialogue*> nonMatchingDialogues;
	if (inputTriggerType == DIALOGUE_TRIGGER::TURN_BASED) {
		if (battleSys) {
			int roundIndex = battleSys->roundManage.roundCounter;
			// bounce back if currentDialogue is already playing dialogue of that turn
			// NOTE: turn 3 is retriggered, turn 4 is not??
			if (currentDialogue && (currentDialogue->triggerType == inputTriggerType) && (currentDialogue->roundTrigger == roundIndex)) {
				return;
			}
			else {
				dialogueQueue.push(currentDialogue);
			}
			while (!dialogueQueue.empty()) {
				if ((dialogueQueue.top()->triggerType != inputTriggerType) || (dialogueQueue.top()->roundTrigger != roundIndex) || (dialogueQueue.top()->isTriggered))
				{
					nonMatchingDialogues.push_back(dialogueQueue.top());
					dialogueQueue.pop();
				}
				else
					break;
			}
			// if match pass dialogue pointer to currentDialogue
			if (!dialogueQueue.empty() && (dialogueQueue.top()->triggerType == inputTriggerType) && (dialogueQueue.top()->roundTrigger == roundIndex)) {
				currentDialogue = dialogueQueue.top();
			}
			// push non-matching dialogues back into queue
			for (Dialogue* dialogue : nonMatchingDialogues) {
				dialogueQueue.push(dialogue);
				dialogueQueue.pop();
			}
		}
	}
	// additional clause for health, if the health threshold has been hit
	/*else if (inputTriggerType == DIALOGUE_TRIGGER::HEALTH_BASED) {
	}*/
	else if (inputTriggerType != DIALOGUE_TRIGGER::DEFAULT) {
		// TODO: to check for turn and health counters?
		while (!dialogueQueue.empty() && (dialogueQueue.top()->triggerType != inputTriggerType)) {
			nonMatchingDialogues.push_back(dialogueQueue.top());
			dialogueQueue.pop();
		}
		// if match pass dialogue pointer to currentDialogue
		if (!dialogueQueue.empty() && (dialogueQueue.top()->triggerType == inputTriggerType)) {
			currentDialogue = dialogueQueue.top();
		}
		// push non-matching dialogues back into queue
		for (Dialogue* dialogue : nonMatchingDialogues) {
			dialogueQueue.push(dialogue);
		}
	}
	else {
		currentDialogue = dialogueQueue.top();
	}

	if (currentDialogue && !currentDialogue->isActive) {
		currentDialogue->isActive = 1;
		currentDialogue->viewingIndex = 0;
		static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
		if (animationArray.HasComponent(entity)) {
			animationArray.GetData(entity).Start("Launch", entity);
		}
		battleSys->dialogueCalled = true;
	}
}

void DialogueHUD::AddDialogue(Dialogue dialogue, bool& result) {
	int index{ 0 };
	for (Dialogue const& d : dialogues) {
		if (&d == currentDialogue) {
			break;
		}
		index++;
	}
	if (dialogues.size() < dialogues.capacity()) {
		dialogues.push_back(dialogue);
		currentDialogue = &dialogues[index];
		dialogueQueue.push(&dialogue);

		result = true;
	}	
}

void DialogueHUD::RemoveDialogue(int index) {
	// step 1: check and update currentDialogue pointer
	if (currentDialogue == &dialogues[index]) {
		currentDialogue = nullptr; // clear away currentDialogue pointer
	}

	// step 2: remove from dialogueQueue
	std::vector<Dialogue*> tempQueue;
	while (!dialogueQueue.empty()) {
		Dialogue* front = dialogueQueue.top();
		dialogueQueue.pop();
		if (front != &dialogues[index]) {
			tempQueue.push_back(front);
		}
	}
	for (Dialogue* ptr : tempQueue) {
		dialogueQueue.push(ptr);
	}

	// step 3: remove from dialogues vector
	if (index >= 0 && index < dialogues.size()) {
		dialogues.erase(dialogues.begin() + index);
	}
}

/*!
* \brief DialogueHUD JumpNextLine
*
* Triggers next line of dialogue, sets isActive to false if no more lines
*
*/
void DialogueHUD::JumpNextLine(Entity entity) {
	if (!currentDialogue) {
		return;
	}

	currentDialogue->viewingIndex++;
	if (currentDialogue->viewingIndex > currentDialogue->dialogueLines.size() - 1) {
		currentDialogue->isActive = 0;
		currentDialogue->isTriggered = 1;
		currentDialogue->viewingIndex--;

		BattleSystem* battleSys = events.GetBattleSystem();
		if (battleSys) {
			// break soft lock cycle, reset dialogueCalled
			battleSys->dialogueCalled = false;
		}

		static auto& animationArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<AnimationSet>() };
		if (animationArray.HasComponent(entity)) {
			if (currentDialogue->triggerType == DIALOGUE_TRIGGER::HEALTH_BASED) {
				BattleSystem* battleSys = events.GetBattleSystem();
				battleSys->activeCharacter->stats.health = 0.5f * battleSys->activeCharacter->stats.maxHealth;
				battleSys->ProcessDamage();
			}
			animationArray.GetData(entity).Start("Exit", entity);
		}
	}
}

/*!
* \brief DialogueHUD Update
*
* Runs every game loop, does event handling for mouse click
*
*/
void DialogueHUD::Update(Model& modelData, Entity entity) {
	// get cursorPos, compare with pos in Transform, return if no match
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::UICOMPONENT]) {
		switch (msg.type) {
		case(TYPE::MOUSE_MOVE):
			uiMousePos = { msg.posX, msg.posY };
			break;
		case(TYPE::MOUSE_CLICK):
			if (IsWithinObject(modelData, uiMousePos)) {
				//on click event trigger (outside edit mode)
				if (GetCurrentSystemMode() == SystemMode::RUN && currentDialogue && currentDialogue->dialogueLines.size()) {
					JumpNextLine(entity);
				}
			}
			break;
		}
	}
}

/*!
* \brief DialogueHUD EnforceAlignment
*
* Auto left-aligns child DialogueSpeaker TextLabel to parent based on text width
*
*/
void DialogueHUD::EnforceAlignment(const Size& parentSizeData, Size& childSizeData, TextLabel& childTextLabelData, Child& childData) {
	// take main dialogue box's position as anchor, ensure child label is aligned (with auto width)
	if (childTextLabelData.textWrap != UI_TEXT_WRAP::AUTO_WIDTH) {
		childTextLabelData.textWrap = UI_TEXT_WRAP::AUTO_WIDTH;
	}
	childData.offset.position.x = (-0.5f * parentSizeData.width) + (0.5f * childSizeData.width);
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