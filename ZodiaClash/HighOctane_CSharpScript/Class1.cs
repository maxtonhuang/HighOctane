﻿namespace CSharpScript
{
    using System;

    public class MyScriptClass
    {
        public static void MyScriptMethod()
        {
            Console.WriteLine("Hello from MyScriptMethod TESTINGGG!");
            
        }

        public static void Start()
        {
            Console.WriteLine("Hello from start method in C#");
        }
        

        public static string StringTest()
        {
            string bufferData = "This is the buffer data from C#";
            return bufferData;
        }
        public static void Stop()
        {
            Console.WriteLine("Hello from stop method in C#");
        }
    }

}