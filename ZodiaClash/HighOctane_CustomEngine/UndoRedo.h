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
*	@file		UndoRedo.h
*
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		10 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file is the definitions for the Undo Feature in our editor.
*   !-----M3-----!
*   - Allowed Undo of transform(position,scale,rotation)
*   - Allowed Undo for Add and removing objects
*
******************************************************************************/
#pragma once
#include <deque>
#include "EntityFactory.h"
#include "ECS.h"
#include "Components.h"
enum class ACTION
{
    NONE,
    TRANSFORM,
    ADDENTITY,
    DELENTITY,
    SIZE,
    TEXTURE,
    ADDCOMP,
    DELCOMP
};

class UndoRedo {
public:
/*!***********************************************************************
 \brief
    A function to record what action is being done in the editor, such as changing transform 
    or changing texture will be stored and recorded
 \param [in] Entity 
    The current entity being edited
 \param [in] Action
    An enum class of action being change such as TRANSFORM, ADDENTITY
 \return
    void
 *************************************************************************/
    void RecordCurrent(Entity entity, ACTION action);
/*!***********************************************************************
\brief
   A function to record what action is being done in the editor
   This function differs at it is meant for the adding and removing of components from the editor.
   The component is being stored in a string to be access via typeMap
\param [in] Entity
   The current entity being edited
\param [in] Action
   An enum class of action being change such as TRANSFORM, ADDENTITY
\param [in] component
   The string representing the component
\return
   void
*************************************************************************/
    void RecordComponent(Entity entity, ACTION action, std::string component);
/*!***********************************************************************
    \brief
        Function to undo the recorded entity and action
        \return
        void
*************************************************************************/
    void Undo();
/*!***********************************************************************
   \brief
       Function to redo the recorded entity and action
       \return
       void
   *************************************************************************/
    void Redo();
/*!***********************************************************************
   \brief
       Function to return the undo stack size
       \return
       size_t stack size
   *************************************************************************/
    size_t UndoSize();
 /*!***********************************************************************
\brief
    Function to return the redo stack size
    \return
    size_t stack size
*************************************************************************/
    size_t RedoSize();
/*!***********************************************************************
\brief
   A helper function to see what layer the entity is in. In order to properly use
   the delete layer function in the layer panel of the level editor
\param [in] layer
    current layer to compoare with the entities of both stacks
\return
   void
*************************************************************************/
    void Layer(size_t layer);
/*!***********************************************************************
\brief
    Function to return the transform of the top of the stack
    \return
    Transform
*************************************************************************/
    Transform CheckFrontTransform();
/*!***********************************************************************
\brief
   A helper function to check if the entity is not in the undostack
\param [in] entity 
    the entity to check
\return
   bool true if it is not in stack
*************************************************************************/
    bool notInUndoStack(size_t checkEntity);
/*!***********************************************************************
\brief
   A helper function to check if the entity is not in the redostack
\param [in] entity 
    the entity to check
\return
   bool true if it is not in stack
*************************************************************************/
    bool notInRedoStack(size_t checkEntity);
/*!***********************************************************************
\brief
   A helper function to  pop the front of the stack
\return
   void
*************************************************************************/
    void StackPopFront();
/*!***********************************************************************
\brief
   A helper function to  find the entity within the stack
\param [in] entity
\return
   bool true if is within stack
*************************************************************************/
    bool Find(Entity entity);
/*!***********************************************************************
\brief
   A helper function to  clear both stack
\return
   void
*************************************************************************/
    void Clear();
    bool undoFlag = false;
private:
    struct EntityChanges {
        Entity entity{};
        ACTION action{};
        Transform transform{};
        Transform prevTransform{};
        Size size{};
        Size prevSize{};
        Tex tex{};
        Tex prevTex{};
        size_t layerIndex{};
        std::string component;
    };

    std::deque<EntityChanges> undoStack;
    std::deque<EntityChanges> redoStack;
};

extern UndoRedo undoRedo;