using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public static class Debug
{
    public static void Trace(string message)
    {
        InternalCalls.DebugTrace(message);
    }

    public static void Info(string message)
    {
        InternalCalls.DebugInfo(message);
    }
    
    public static void Warning(string message)
    {
        InternalCalls.DebugWarning(message);
    }

    public static void Error(string message)
    {
        InternalCalls.DebugError(message);
    }

    public static void Fatal(string message)
    {
        InternalCalls.DebugFatal(message);
    }

    public static void Log(string message)
    {
        InternalCalls.Log(message);
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

    public static bool GetKeyDown(INFO key)
    {
        return InternalCalls.GetKeyDown(key);
    }
}
