using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Transactions;


public class Entity
{

    protected Entity() { ID = 0; }

    public Entity(UInt32 id)
    {
        ID = id;
    }

    public readonly UInt32 ID = 0;

    public Vector2 Translation
    {
        get
        {
            InternalCalls.EntityGetPosition(ID , out Vector2 position);
            return position;
        }
        set
        {
            InternalCalls.EntitySetPosition(ID, ref value);
        }
    }
    
    public float DeltaTime
    {
        get
        {
            InternalCalls.GetDeltaTime(out float deltaTime);
            return deltaTime;
        }
    }

    public Vector2 Force
    {
        get
        {
            InternalCalls.EntityGetForce(ID, out Vector2 force);
            return force;
        }

        set
        {
            InternalCalls.EntitySetForce(ID, ref value);
        }
    }
    public float Rotation
    {
        get
        {
            InternalCalls.EntityGetRotation(ID, out float force);
            return force;
        }

        set
        {
            InternalCalls.EntitySetRotation(ID, ref value);
        }
    }

    public float Scale
    {
        get
        {
            InternalCalls.EntityGetScale(ID, out float force);
            return force;
        }

        set
        {
            InternalCalls.EntitySetScale(ID, ref value);
        }
    }


    public bool HasComponent<T>() where T : Component, new()
    {
        Type componentType = typeof(T);
        return InternalCalls.EntityHasComponent(ID);
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

