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
#include <map>
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

/*!
 * \brief Represents the type of a field in a script.
 *
 * This enumeration defines various types of fields that can be used in scripts, such as basic data types (int, float, etc.) and more complex types like vectors and entities.
 */
enum class ScriptFieldType
{
	None = 0,
	Float, Double,
	Bool, Char, Byte, Short, Int, Long,
	UByte, UShort, UInt, ULong,
	Vector2, Vector3, Vector4,
	Entity
};

/*!
 * \brief Encapsulates information about a script field.
 *
 * This structure holds information about a field within a script, including its type, name, and a pointer to the corresponding MonoClassField.
 */
struct ScriptField
{
	ScriptFieldType Type{};
	std::string Name{};

	MonoClassField* ClassField{};
};

/*!
 * \brief Represents an instance of a script field with data storage.
 *
 * This structure extends ScriptField by adding data storage capabilities. It provides methods to get and set the value of the field, ensuring type safety and size constraints.
 */
struct ScriptFieldInstance
{
	ScriptField Field{};

	ScriptFieldInstance()
	{
		memset(m_Buffer, 0, sizeof(m_Buffer));
	}

	/*!
	 * \brief Retrieves the value of the field.
	 *
	 * Template function to get the value of the field. It ensures that the size of the type T is within the allowable limit.
	 *
	 * \return The value of the field.
	 */
	template<typename T>
	T GetValue()
	{
		static_assert(sizeof(T) <= 16, "Type too large!");
		return *(T*)m_Buffer;
	}

	/*!
	 * \brief Sets the value of the field.
	 *
	 * Template function to set the value of the field. It ensures that the size of the type T is within the allowable limit.
	 *
	 * \param value The value to set for the field.
	 */
	template<typename T>
	void SetValue(T value)
	{
		static_assert(sizeof(T) <= 16, "Type too large!");
		memcpy(m_Buffer, &value, sizeof(T));
	}
private:
	uint8_t m_Buffer[16];

	friend class ScriptEngine;
	friend class ScriptInstance;
};


using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

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

	/*!
	 * \brief Retrieves the map of fields of the associated script class for the ScriptClass.
	 *
	 * Returns the map of fields of the script class associated with this ScriptClass.
	 *
	 * \return std::map <std::string, ScriptField>  The map of fields of the script class.
	 *
	 */
	const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }

	/*!
	 * \brief Retrieves the monoClass of the associated script class for the ScriptClass.
	 *
	 * Returns the monoClass of the script class associated with this ScriptClass.
	 *
	 * \return MonoClass* The monoClass of the script class.
	 *
	 */
	MonoClass* GetMonoClass() const {
		return m_MonoClass;
	}

private:
	std::string m_ClassNamespace; /*!< The namespace of the script class. */
	std::string m_ClassName; /*!< The name of the script class. */
	MonoClass* m_MonoClass = nullptr; /*!< The MonoClass instance representing the script class. */
	std::map<std::string, ScriptField> m_Fields;

	friend class ScriptEngine;
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
	 * \brief Retrieves the script class associated with this ScriptInstance.
	 *
	 * This function returns a pointer to the ScriptClass that represents the script class associated with this ScriptInstance.
	 *
	 * \return A pointer to the ScriptClass representing the script class.
	 */
	std::shared_ptr<ScriptClass> GetScriptClass() { return m_ScriptClass; }

	/*!
	 * \brief Retrieves the value of a field by name.
	 *
	 * This template function retrieves the value of a field specified by its name. It ensures that the size of the type T is within the allowable limit. If the field is not found or the operation fails, a default-constructed object of type T is returned.
	 *
	 * \param name The name of the field whose value is to be retrieved.
	 * \return The value of the field, or a default-constructed object of type T if the operation fails.
	 */
	template<typename T>
	T GetFieldValue(const std::string& name)
	{
		static_assert(sizeof(T) <= 16, "Type too large!");

		bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
		if (!success)
			return T();

		return *(T*)s_FieldValueBuffer;
	}

	/*!
	 * \brief Sets the value of a field by name.
	 *
	 * This template function sets the value of a field specified by its name. It ensures that the size of the type T is within the allowable limit.
	 *
	 * \param name The name of the field whose value is to be set.
	 * \param value The value to set for the field.
	 */
	template<typename T>
	void SetFieldValue(const std::string& name, T value)
	{
		static_assert(sizeof(T) <= 16, "Type too large!");

		SetFieldValueInternal(name, &value);
	}

	/*!
	 * \brief Retrieves the managed object associated with this instance.
	 *
	 * This function returns a pointer to the MonoObject that represents the managed object associated with this instance in the scripting environment.
	 *
	 * \return A pointer to the MonoObject representing the managed object.
	 */
	MonoObject* GetManagedObject() { return m_Instance; }


private:
	/*!
	 * \brief Retrieves the value of a field by name into a buffer.
	 *
	 * This function attempts to retrieve the value of a field specified by its name and stores it in the provided buffer. It returns true if the operation is successful.
	 *
	 * \param name The name of the field whose value is to be retrieved.
	 * \param buffer A pointer to the buffer where the field value will be stored.
	 * \return bool True if the field value was successfully retrieved, false otherwise.
	 */
	bool GetFieldValueInternal(const std::string& name, void* buffer);

	/*!
	 * \brief Sets the value of a field by name.
	 *
	 * This function attempts to set the value of a field specified by its name using the value provided. It returns true if the operation is successful.
	 *
	 * \param name The name of the field whose value is to be set.
	 * \param value A pointer to the value to be set for the field.
	 * \return bool True if the field value was successfully set, false otherwise.
	 */
	bool SetFieldValueInternal(const std::string& name, const void* value);

	/*!
	 * \brief Retrieves the name of the associated script class for the ScriptInstance.
	 *
	 * This method returns the full name of the script class associated with this ScriptInstance. It provides a means to identify the script class at runtime.
	 *
	 * \return std::string The name of the script class.
	 */
	std::string GetScriptName() const;

	/*!
	 * \brief Retrieves the MonoObject instance associated with this ScriptInstance.
	 *
	 * This method returns a pointer to the MonoObject that represents the managed object associated with this ScriptInstance in the scripting environment. It provides direct access to the underlying managed object.
	 *
	 * \return MonoObject* A pointer to the MonoObject representing the managed object.
	 */
	MonoObject* GetInstance() const {
		return m_Instance;
	}



private:

	std::shared_ptr<ScriptClass> m_ScriptClass = nullptr; /*!< A shared pointer to the associated ScriptClass instance. */
	MonoObject* m_Instance = nullptr; /*!< The instance of the script class. */
	MonoMethod* m_Constructor = nullptr; /*!< The constructor method of the script class. */
	MonoMethod* m_StartMethod = nullptr; /*!< The method to invoke when the script is created. */
	MonoMethod* m_UpdateMethod = nullptr; /*!< The method to invoke for script updates. */

	inline static char s_FieldValueBuffer[16];

	friend class ScriptEngine;
	friend struct ScriptFieldInstance;
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
	std::unordered_map<Entity, ScriptFieldMap> EntityScriptFields;

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

	/*!
	 * \brief Retrieves the script instance associated with a specific entity.
	 *
	 * This static method returns a shared pointer to the ScriptInstance associated with the given entity ID. It allows access to the script functionality bound to the entity.
	 *
	 * \param entityID The ID of the entity whose script instance is to be retrieved.
	 * \param i An integer parameter, possibly used for indexing or specifying a version.
	 * \return std::shared_ptr<ScriptInstance> A shared pointer to the associated ScriptInstance.
	 */
	static std::shared_ptr<ScriptInstance> GetEntityScriptInstance(Entity entityID, int i);

	/*!
	 * \brief Sets the default path for script-related operations.
	 *
	 * This static method sets the default path used by the script engine for loading and managing scripts.
	 *
	 * \param path The default path to be set.
	 */
	static void SetDefaultPath(std::string path);

	/*!
	 * \brief Retrieves the singleton instance of the ScriptEngineData.
	 *
	 * This static method returns a pointer to the ScriptEngineData instance. If the instance does not exist, it is created. This method ensures that only one instance of ScriptEngineData exists (singleton pattern).
	 *
	 * \return ScriptEngineData* A pointer to the singleton instance of ScriptEngineData.
	 */
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
	static std::string defaultPath;
	static ScriptEngineData* scriptData;

	// Friend
	friend class ScriptClass;
	friend class ScriptInstance;
};

/*!
 * \brief Converts a ScriptFieldType to its string representation.
 *
 * This inline function maps a ScriptFieldType enum value to its corresponding string representation. It is useful for serialization, logging, or UI display.
 *
 * \param fieldType The ScriptFieldType to be converted.
 * \return const char* The string representation of the ScriptFieldType.
 */
inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
{
	switch (fieldType)
	{
	case ScriptFieldType::None:    return "None";
	case ScriptFieldType::Float:   return "Float";
	case ScriptFieldType::Double:  return "Double";
	case ScriptFieldType::Bool:    return "Bool";
	case ScriptFieldType::Char:    return "Char";
	case ScriptFieldType::Byte:    return "Byte";
	case ScriptFieldType::Short:   return "Short";
	case ScriptFieldType::Int:     return "Int";
	case ScriptFieldType::Long:    return "Long";
	case ScriptFieldType::UByte:   return "UByte";
	case ScriptFieldType::UShort:  return "UShort";
	case ScriptFieldType::UInt:    return "UInt";
	case ScriptFieldType::ULong:   return "ULong";
	case ScriptFieldType::Vector2: return "Vector2";
	case ScriptFieldType::Vector3: return "Vector3";
	case ScriptFieldType::Vector4: return "Vector4";
	case ScriptFieldType::Entity:  return "Entity";
	}
	return "None";
}

/*!
 * \brief Converts a string to its corresponding ScriptFieldType.
 *
 * This inline function maps a string to its corresponding ScriptFieldType enum value. It is useful for deserialization or interpreting user input.
 *
 * \param fieldType A string view representing the field type.
 * \return ScriptFieldType The corresponding ScriptFieldType enum value.
 */
inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
{
	if (fieldType == "None")    return ScriptFieldType::None;
	if (fieldType == "Float")   return ScriptFieldType::Float;
	if (fieldType == "Double")  return ScriptFieldType::Double;
	if (fieldType == "Bool")    return ScriptFieldType::Bool;
	if (fieldType == "Char")    return ScriptFieldType::Char;
	if (fieldType == "Byte")    return ScriptFieldType::Byte;
	if (fieldType == "Short")   return ScriptFieldType::Short;
	if (fieldType == "Int")     return ScriptFieldType::Int;
	if (fieldType == "Long")    return ScriptFieldType::Long;
	if (fieldType == "UByte")   return ScriptFieldType::UByte;
	if (fieldType == "UShort")  return ScriptFieldType::UShort;
	if (fieldType == "UInt")    return ScriptFieldType::UInt;
	if (fieldType == "ULong")   return ScriptFieldType::ULong;
	if (fieldType == "Vector2") return ScriptFieldType::Vector2;
	if (fieldType == "Vector3") return ScriptFieldType::Vector3;
	if (fieldType == "Vector4") return ScriptFieldType::Vector4;
	if (fieldType == "Entity")  return ScriptFieldType::Entity;

	return ScriptFieldType::None;
}
