﻿//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
#nullable disable

using System.Linq.Expressions;

public abstract class Component 
{
    public Entity Entity { get; internal set; }
}

public class TransformComponent : Component
{
    public Vector2 Translation
    {
        get
        {
            InternalCalls.TransformGetPosition(Entity.ID, out Vector2 translation);
            return translation;
        }
        set
        {
            InternalCalls.TransformSetPosition(Entity.ID, ref value);
        }
    }

    public Vector2 Force
    {
        get
        {
            InternalCalls.TransformGetForce(Entity.ID, out Vector2 force);
            return force;
        }

        set
        {
            InternalCalls.TransformSetForce(Entity.ID, ref value);
        }
    }

}

//public class Transform : Component
//{
//    get
//        {
//            InternalCalls.TransformGetTranslation(/*Entity.ID, */out Vector3 translation);
//            return translation;
//        }
//set
//        {
//            InternalCalls.TransformSetTranslation(/*Entity.ID, */ref value);
//        }
//}


//public class Components
//{
//    public Entity Entity { get; internal set; }
//}
//public struct Transform
//{
//    public Vector2 position;
//    public float rotation;
//    public float scale;
//    public Vector2 velocity;
//    public float radius;
//    public Vector2 halfDimensions;
//    public bool isStatic;
//    public float mass;
//    public float inverseMass;
//    public Vector2 acceleration;
//    public Vector2 force;
//}

//    public struct Collider
//    {
//        public enum SHAPE_ID
//        {
//            SHAPE_BOX,
//            SHAPE_CIRCLE,
//            NUM_OF_SHAPES
//        }
//        public SHAPE_ID bodyShape;
//    }

//    public struct Size
//    {
//        public float width;
//        public float height;
//    }

//    // NOT NEEDED, TO STORE IN MODEL COMPONENT
//    public struct Color
//    {
//        public Vector4 color;
//    }

//    public struct Visible
//    {
//        public bool isVisible;
//    }

//    public struct MainCharacter
//    {
//        // empty by design
//    }

//    public struct Circle
//    {
//        public float radius;
//    }

//    public struct AABB
//    {
//        public Vector2 min;
//        public Vector2 max;
//        public Vector2 extents;
//    }

//    public struct Master
//    {
//        // empty by design
//    }

//    public struct Clone
//    {
//        // empty by design
//    }

//    public struct Name
//    {
//        public string name;
//        public bool selected;
//    }

//    public struct Movable
//    {
//        // empty by design
//    }

//    public struct Screen
//    {
//        public bool isActive;
//    }

//    public struct Script
//    {
//        public string className;
//        public List<string> scriptNameVec;
//        public List<string> scriptNameVecForImGui;
//    }

//    public struct Tag
//    {
//        public string tag;
//    }
//}