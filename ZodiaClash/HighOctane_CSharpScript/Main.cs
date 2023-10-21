using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


public struct Vector3
{
    public float X, Y, Z;

    public Vector3(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }
}

public class Main
{
    public float FloatVar { get; set; }
    public Main()
    {
        Console.WriteLine("Main constructor");
        Debug.Log("Hello this is the custom log that oliver will use from C#");
        Vector3 pos = new Vector3(1, 2, 3);
        Vector3 result = Debug.LogVector3(pos);
        Console.WriteLine(result.X + ", " + result.Y + ", " + result.Z);                
        //Debug.Log($"Result: {result.X}, {result.Y}, {result.Z}");
        //Debug.Log("Main" constructor");
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
