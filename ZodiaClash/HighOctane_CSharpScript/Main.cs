using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


public class Main
{
    public float FloatVar { get; set; }
    public Main()
    {
        Console.WriteLine("Main constructor");
        //Debug.Log("Main constructor");
    }

    public void PrintMessage()
    {
        Console.WriteLine("Hello World from C#");
        //Debug.Log("Hello World from C#");
    }
    public void PrintInt(int value)
    {
        Console.WriteLine($"C# says: {value}");
        //Debug.Log($"C# says: {message}");
    }
    public void PrintCustomMessage(string message)
    {
        Console.WriteLine($"C# says {message}");
        //Debug.Log($"C# says: {message}");
    }
}
