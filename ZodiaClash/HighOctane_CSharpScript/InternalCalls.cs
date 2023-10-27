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

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern void EntityGetTranslation(out Vector3 result);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern void EntitySetTranslation(ref Vector3 value);

    // Adds the get axis function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern int GetAxisHorizontal();

    // Adds the get axis function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern int GetAxisVertical();

    #region Transform
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformGetTranslation(UInt32 entity, out Vector2 translation);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformSetTranslation(UInt32 entity, ref Vector2 translation);
    #endregion

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformGetForce(UInt32 entity, out Vector2 translation);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void TransformSetForce(UInt32 entity, ref Vector2 translation);
    #region Entity
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool EntityHasComponent(UInt32 entity, Type componentType);

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