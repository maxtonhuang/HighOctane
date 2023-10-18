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
        // Test
    }

    public static bool GetKeyDown(INFO key)
    {
        return InternalCalls.GetKeyDown(key);
    }
}
