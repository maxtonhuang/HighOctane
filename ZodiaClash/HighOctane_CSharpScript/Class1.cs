using System;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public static class InternalCalls
{
    // Adds the log function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern void Log(string message);

    // Adds the print function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Print();

    // Adds the Get Key Down function into internal call table
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal static extern bool GetKeyDown(INFO key);
}



public class MyScriptClass
{

    //public const string CppFunctionsDLL = @"..\..\..\..\bin\Debug-x64\HighOctane_CustomEngine.dll";

    //[DllImport(CppFunctionsDLL, CallingConvention = CallingConvention.Cdecl)]
    //public static extern int AddNumbers(int a, int b);

    //[DllImport(CppFunctionsDLL, CallingConvention = CallingConvention.Cdecl)]
    //public static extern int SubtractNumbers(int a, int b);

    //static void Main(string[] args)
    //{
    //    int input1, input2;

    //    Console.Write("Input number 1: ");
    //    if (!int.TryParse(Console.ReadLine(), out input1))
    //    {
    //        Console.WriteLine("Invalid input");
    //        input1 = 5;
    //    }

    //    Console.Write("Input number 1: ");
    //    if (!int.TryParse(Console.ReadLine(), out input2))
    //    {
    //        Console.WriteLine("Invalid input");
    //        input2 = 10;
    //    }

    //    int output = AddNumbers(input1, input2);

    //    Console.WriteLine($"Output: " + output);
    //}

    //[DllImport("__Internal")]
    
    public static void PrintFromCSharp()
    {
        // Call the C++ function through the C# wrapper
        InternalCalls.Print();
    }

    // Function to be called from C++ code
    public static void TestInterop(int input)
    {
        int output = input * 2;
        Debug.Log("C# calling C++ code. Your number was " + output);
        Debug.Log("Hi from oliver");
        Testing();

        // This will eventually be moved to a function that is updating every frame
        // Need to test whether it is working, because I am not sure 
        
        // On top here
    }

    
    public static void Testing()
    {

        if (Debug.GetKeyDown(INFO.KEY_SPACE))
        {
            Debug.Log("Space key pressed (From C#)");
        }

        if (Debug.GetKeyDown(INFO.KEY_W))
        {
            Debug.Log("W key pressed (From C#)");
        }


    }

    public static void Start()
    {
        Console.WriteLine("Hello from start method in C#");
    }
        
    public static void Stop()
    {
        Console.WriteLine("Hello from stop method in C#");
    }
}


