using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

public enum INFO
{
    KEY_SPACE = 32,
    KEY_0 = 48,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_A = 65,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_ESC = 256,
    KEY_ENTER,
    KEY_RIGHT = 262,
    KEY_LEFT,
    KEY_DOWN,
    KEY_UP,
    MOUSE_LEFT = 501,
    MOUSE_RIGHT,
    NONE = 999
}

public static class InternalCalls
{

    // Adds the Get Key Down function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern bool GetKeyDown(INFO key);

    // Adds the log function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern void Log(string message);

    // Adds the log function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern void LogVector3(ref Vector3 param, out Vector3 result);

    // Adds the get axis function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern int GetAxisHorizontal();

    // Adds the get axis function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern int GetAxisVertical();

    #region Transform
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntityGetPosition(UInt32 entity, out Vector2 position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntitySetPosition(UInt32 entity, ref Vector2 position);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntityGetForce(UInt32 entity, out Vector2 force);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntitySetForce(UInt32 entity, ref Vector2 force);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntityGetRotation(UInt32 entity, out float outRotation);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntitySetRotation(UInt32 entity, ref float rotation);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntityGetScale(UInt32 entity, out float outScale);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntitySetScale(UInt32 entity, ref float scale);
    #endregion


    #region deltaTime
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void GetDeltaTime(out float outDeltaTime);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void GetFixedDeltaTime(out float outFixedDeltaTime);

    #endregion

    #region Entity
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool EntityHasComponent(UInt32 entity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntityAddComponent(UInt32 entity, Type componentType);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void EntityRemoveComponent(UInt32 entity);

    //[MethodImplAttribute(MethodImplOptions.InternalCall)]
    //internal extern static Type EntityGetComponent(UInt32 entity);
    //[MethodImplAttribute(MethodImplOptions.InternalCall)]
    //internal extern static ComponentType EntityGetComponentType(UInt32 entity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool EntityIsComponentTypeRegistered(UInt32 entity);
    #endregion
}