using System;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

// Doesn't show on inspector
// [SerializeField] private GameObject pauseScreen;

// Show on inspector
// [SerializeField] pubcli GameObject pauseScreen;

// Create a class that all inherit from
public abstract class MonoBehaviour
{
    virtual public void Start()
    {

    }

    virtual public void Update()
    {

    }

    // Can have a on shut down
    virtual public void ShutDown()
    {

    }

    // Can have a on triggered, or even a set active?
    virtual public void OnTriggered()
    {

    }

    virtual public void SetActive()
    {

    }

}


// In mono, detect the base class, then all the one inherit will be detected

public class MyScriptClass : MonoBehaviour
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

    public override void Start()
    {

    }
    
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

    //public static void Start()
    //{
    //    Console.WriteLine("Hello from start method in C#");
    //}
        
    //public static void Stop()
    //{
    //    Console.WriteLine("Hello from stop method in C#");
    //}
}


