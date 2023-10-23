using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    internal class Player : Entity
    {

        void OnCreate()
        {
            //Console.WriteLine("Player OnCreate");

        }

        void OnUpdate()
        {
            //Console.WriteLine($"Player OnUpdate:");
            if (Debug.GetKeyDown(INFO.KEY_W))
            {
                Debug.Log("W is pressed from C#");
            }
            if (Debug.GetKeyDown(INFO.KEY_A))
            {
                Debug.Log("A is pressed from C#");
            }
            if (Debug.GetKeyDown(INFO.KEY_S))
            {
                Debug.Log("S is pressed from C#");
            }
            if (Debug.GetKeyDown(INFO.KEY_D))
            {
                Debug.Log("D is pressed from C#");
            }
        }
    }

    internal class PlayerController : Entity
    {
        void OnCreate()
        {
            Console.WriteLine("PlayerController OnCreate");
        }

        void OnUpdate()
        {
            Console.WriteLine($"PlayerController OnUpdate");
        }
    }
}
