using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


public class Entity
{
    // ID is currently hardcoded, need to find a way to make it not hard coded
    // Use the mono constructor thing that chernos did
    // Disable default constructor
    protected Entity() { ID = 0; }

    public Entity(UInt32 id)
    {
        ID = id;
    }

    public readonly UInt32 ID;

    public Vector2 Translation
    {
        get
        {
            InternalCalls.TransformGetTranslation(ID , out Vector2 translation);
            return translation;
        }
        set
        {
            InternalCalls.TransformSetTranslation(ID, ref value);
        }
    }
    
    public Vector2 Force
    {
        get
        {
            InternalCalls.TransformGetForce(ID, out Vector2 force);
            return force;
        }

        set
        {
            InternalCalls.TransformSetForce(ID, ref value);
        }
    }


    public bool HasComponent<T>() where T : Component, new()
    {
        Type componentType = typeof(T);
        return InternalCalls.EntityHasComponent(ID, componentType);
    }

    public void AddComponent<T>() where T : Component, new()
    {
        Type componentType = typeof(T);
        InternalCalls.EntityAddComponent(ID, componentType);
    }

    public void RemoveComponent<T>() where T : Component, new()
    {
        Type componentType = typeof(T);
        InternalCalls.EntityRemoveComponent(ID);
    }

    public T? GetComponent<T>() where T : Component, new()
    {
        if (!HasComponent<T>())
            return null;

        T component = new T() { Entity = this };
        return component;
    }

}

