using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public static class Debug
{
    public static void Log(string message)
    {
        InternalCalls.Log(message);
    }

    public static bool GetKeyDown(INFO key)
    {
        return InternalCalls.GetKeyDown(key);
    }

    public static Vector3 LogVector3(Vector3 vector)
    {
        InternalCalls.LogVector3(ref vector, out Vector3 result);
        return result;
    }
}

public static class Input
{
    public static int GetAxisHorizontal()
    {
        return InternalCalls.GetAxisHorizontal();
    }

    public static int GetAxisVertical()
    {
        return InternalCalls.GetAxisVertical();
    }
}
