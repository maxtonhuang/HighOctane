using System.Runtime.CompilerServices;

namespace CSharpScript
{
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

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


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Print();
        public static void PrintFromCSharp()
        {
            // Call the C++ function through the C# wrapper
            Print();
        }

        // Function to be called from C++ code
        public void TestInterop(int input)
        {
            Log("C# calling C++ code. Your number was " + input);
        }

        // Function written in unmanaged (C++) land.
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Log(string message);
        public static void MyScriptMethod()
        {
            Console.WriteLine("Hello from MyScriptMethod TESTINGGG!");
            
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

}


