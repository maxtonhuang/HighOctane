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
*	@file		ScriptEngine.h
*
*	@author		Koh Wen Yuan
*
*	@email		k.wenyuan\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		20 October 2023
*
* *****************************************************************************
*
*	@brief		Header files for the scripting engine
*
*	This file contains the declarations of the functions for the scripting engine
******************************************************************************/

#include <iostream>
#include <filesystem>
#include <string>
#include "ECS.h"
#include "Global.h"
#include "Message.h"
#include "Components.h"
#include "Debuglog.h"

#pragma once

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoAssemblyName MonoAssemblyName;
	typedef struct _MonoClassField MonoClassField;

}

struct ScriptVariableInfo {
	std::string className;
	int typeName;
	std::string variableName;
	uint32_t fieldType;
};

/*!
 * \brief Represents a class in the script engine.
 *
 * This class encapsulates information and functionality related to a script class, including class namespace, name, and relevant MonoClass instance.
 */
class ScriptClass {
public:

	/*!
		 * \brief Constructor for the ScriptClass.
		 *
		 * Initializes the ScriptClass with default values.
		 *
		 */
	ScriptClass() = default;
	ScriptClass(const std::string& classNamespace, const std::string& className);
	/*!
	 * \brief Instantiates the represented script class.
	 *
	 * Creates an instance of the script class represented by this ScriptClass instance using the previously retrieved MonoClass.
	 *
	 * \return Returns a MonoObject pointer to the newly created instance of the script class.
	 */
	MonoObject* Instantiate();

	/*!
	 * \brief Retrieves a method from the script class.
	 *
	 * Fetches a method from the script class by its name and the expected number of parameters.
	 *
	 * \param name The name of the method to retrieve.
	 * \param parameterCount The count of parameters the method expects.
	 * \return Returns a MonoMethod pointer to the identified method from the script class.
	 */
	MonoMethod* GetMethod(const std::string& name, int parameterCount);

	/*!
	 * \brief Invokes a method of the script class on the given instance.
	 *
	 * Invokes a specified method of the script class on the provided instance with the supplied parameters.
	 *
	 * \param instance The instance of the script class to perform the method invocation.
	 * \param method The method to be invoked on the given instance.
	 * \param params The parameters to be passed to the invoked method.
	 * \return Returns a MonoObject pointer, the result of the method invocation.
	 */
	MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

	/*!
	 * \brief Retrieves the namespace of the associated script class for the ScriptClass.
	 *
	 * Returns the namespace of the script class associated with this ScriptClass.
	 *
	 * \return std::string The namespace of the script class.
	 */
	std::string GetMClassNameSpace() const;

	/*!
	 * \brief Retrieves the name of the associated script class for the ScriptClass.
	 *
	 * Returns the name of the script class associated with this ScriptClass.
	 *
	 * \return std::string The name of the script class.
	 */
	std::string GetMClassName() const;

	MonoClass* GetMonoClass() const {
		return m_MonoClass;
	}

private:
	std::string m_ClassNamespace; /*!< The namespace of the script class. */
	std::string m_ClassName; /*!< The name of the script class. */
	MonoClass* m_MonoClass = nullptr; /*!< The MonoClass instance representing the script class. */
};

/*!
 * \brief Represents an instance of a script class.
 *
 * The ScriptInstance class manages the instance of a script class, including its construction and invocation of specific methods.
 */
class ScriptInstance {
public:
	/*!
	 * \brief Constructor for the ScriptInstance.
	 *
	 * Initializes the ScriptInstance with a given script class and an entity, creating an instance of the script class and preparing the entity constructor, as well as the Start and Update methods.
	 *
	 * \param scriptClass A shared pointer to the ScriptClass representing the script.
	 * \param entity The entity with which the script instance is associated.
	 */
	ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity);

	/*!
	 * \brief Invokes the Start method of the script instance.
	 *
	 * Invokes the Start method of the script instance represented by this ScriptInstance.
	 */
	void InvokeOnCreate();

	/*!
	 * \brief Invokes the Update method of the script instance.
	 *
	 * Invokes the Update method of the script instance represented by this ScriptInstance.
	 */
	void InvokeOnUpdate();

	/*!
	 * \brief Retrieves the name of5 the associated script class for the ScriptInstance.
	 *
	 * Returns the full name of the script class associated with this ScriptInstance.
	 *
	 * \return std::string The name of the script class.
	 */
	std::string GetScriptName() const;

	MonoObject* GetInstance() const {
				return m_Instance;
	}


private:

	std::shared_ptr<ScriptClass> m_ScriptClass = nullptr; /*!< A shared pointer to the associated ScriptClass instance. */
	MonoObject* m_Instance = nullptr; /*!< The instance of the script class. */
	MonoMethod* m_Constructor = nullptr; /*!< The constructor method of the script class. */
	MonoMethod* m_StartMethod = nullptr; /*!< The method to invoke when the script is created. */
	MonoMethod* m_UpdateMethod = nullptr; /*!< The method to invoke for script updates. */


};

/*!
 * \brief Reads the binary content of a file and returns the read bytes.
 *
 * Opens the file at the specified path, reads its binary content, and returns the read bytes.
 *
 * \param filepath The path to the file to be read.
 * \param[out] outSize The size of the file read in bytes.
 * \return char* A pointer to the read bytes; returns nullptr if failed or the file is empty.
 */
static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize);

/*!
 * \brief Loads a Mono assembly from the specified file path.
 *
 * Reads the binary content of a file, loads it as a Mono assembly, and returns the assembly.
 *
 * \param assemblyPath The path to the assembly file to load.
 * \return MonoAssembly* The loaded Mono assembly; returns nullptr on failure.
 */
static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath);

class ScriptEngineData {

public:
	MonoDomain* RootDomain = nullptr;
	MonoDomain* AppDomain = nullptr;

	MonoAssembly* CoreAssembly = nullptr;
	MonoImage* CoreAssemblyImage = nullptr;

	ScriptClass EntityClass;

	std::unordered_map<std::string, std::shared_ptr<ScriptClass>> EntityClasses;
	std::unordered_map<Entity, std::vector<std::shared_ptr<ScriptInstance>>> EntityInstances;

	std::vector<ScriptVariableInfo> ScriptInfoVec; // 
};

class ScriptEngine {

public:
	/*!
	 * \brief Initializes the Script Engine.
	 *
	 * This function initializes the script engine, sets up necessary paths for assemblies, loads required assemblies,
	 * adds internal calls for various methods, and identifies methods related to entities.
	 */
	static void Init();

	/*!
	 * \brief Shuts down the Script Engine.
	 *
	 * This method shuts down the Script Engine, performing necessary cleanup and memory deallocation.
	 */
	static void Shutdown();

	/*!
	 * \brief Loads a Mono assembly from the provided file path.
	 *
	 * This function creates an App Domain and loads the Mono assembly from the specified file path. If the provided file path doesn't exist, it attempts to load the default core assembly.
	 *
	 * \param filePath The path to the Mono assembly file.
	 */
	static void LoadAssembly(const std::filesystem::path& filePath);

	static void UnloadAssembly();

	static void HotReloadScript();

	/*!
	 * \brief Checks if the specified entity class exists within the script engine.
	 *
	 * This function checks whether a particular entity class exists within the script engine's EntityClasses.
	 *
	 * \param fullClassName The name of the entity class to check for existence.
	 * \return bool True if the entity class exists, false otherwise.
	 */
	static bool EntityClassExists(const std::string& fullClassName);

	/*!
	 * \brief Initializes the script associated with the entity.
	 *
	 * This function initializes the script associated with the provided entity. It retrieves each script associated with the entity, checks if it exists in the system, and creates an instance of the script class.
	 *
	 * \param entity The entity to initialize the script for.
	 */
	static void ScriptInit(Entity entity);

	/*!
	 * \brief Updates the script of an entity.
	 *
	 * This function triggers the Update event for each ScriptInstance associated with the specified entity.
	 *
	 * \param entity The entity whose scripts are to be updated.
	 */
	static void ScriptUpdate(const Entity& entity);

	/*!
	 * \brief Adds a script to an entity at runtime.
	 *
	 * This function adds a script to the specified entity dynamically at runtime. It checks if the script is already present in the entity's scriptNameVec and adds it to the vector if it's not already there. If added, it creates a new ScriptInstance and associates it with the entity.
	 *
	 * \param entity The entity to which the script will be added.
	 * \param scriptName The name of the script to add.
	 */
	static void AttachScriptToEntity(Entity entity, std::string scriptName);

	/*!
	 * \brief Removes a script from an entity at runtime.
	 *
	 * This function removes a script from the specified entity dynamically at runtime. It checks for the specified script within the entity's list of ScriptInstances. If found, it removes the ScriptInstance and clears the association between the entity and the removed script.
	 *
	 * \param entity The entity from which the script will be removed.
	 * \param scriptName The name of the script to remove.
	 */
	static void RemoveScriptFromEntity(Entity entity, std::string scriptName);

	static void SetScriptProperty(Entity entity, const std::string& className, const std::string& propertyName, void* value);

	static ScriptEngineData* GetInstance() {
		if (!scriptData) {
			scriptData = new ScriptEngineData();
			//printf("ScriptEngineData created\n");
		}
		return scriptData;
	}

private:

	/*!
	 * \brief Initializes the Mono framework and sets up necessary paths.
	 *
	 * This method initializes the Mono framework, setting up the necessary paths for assemblies, and creating the root domain for script execution.
	 */
	static void InitMono();

	/*!
	 * \brief Shuts down the Mono runtime.
	 *
	 * This function performs cleanup procedures for shutting down the Mono runtime. It sets the AppDomain pointer to nullptr and performs the cleanup of the root domain of Mono.
	 */
	static void ShutdownMono();

	/*!
	 * \brief Instantiates a class of a specified type.
	 *
	 * This method creates an instance of a class represented by the given MonoClass object.
	 *
	 * \param classToInstantiate The MonoClass representing the class to be instantiated.
	 * \return Returns a MonoObject pointer to the created instance.
	 */
	static MonoObject* InstantiateClass(MonoClass* classToInstantiate);
	
	/*!
	 * \brief Loads assembly classes into the script engine.
	 *
	 * This function loads the classes from the provided MonoAssembly into the script engine. It populates the EntityClasses map with the classes that inherit from MonoBehaviour and stores the fully-qualified names of these classes.
	 *
	 * \param assembly The assembly from which the classes are loaded.
	 */
	static void LoadAssemblyClasses(MonoAssembly* assembly);

	static ScriptEngineData* scriptData;
	// Friend
	friend class ScriptClass;
	friend class ScriptInstance;
};