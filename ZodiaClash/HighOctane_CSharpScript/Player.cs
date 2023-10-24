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
        public override void OnCreate()
        {
            //Console.WriteLine("Player OnCreate");

        }

        public override void OnUpdate()
        {

            if (Input.GetAxisHorizontal() == 1)
            {
                Debug.Log("Right");
            }
            else if (Input.GetAxisHorizontal() == -1)
            {
                Debug.Log("Left");
            }

            if (Input.GetAxisVertical() == 1)
            {
                Debug.Log("Up");
            }

            else if (Input.GetAxisVertical() == -1)
            {
                Debug.Log("Down");
            }
        }
    }

}
namespace PlayerController
{
    internal class PlayerMovement : Entity
    {
        public override void OnCreate()
        {
            Console.WriteLine("PlayerMovement OnCreate");
        }

        public override void OnUpdate()
        {
            //Console.WriteLine($"PlayerMovement OnUpdate");
        }
    }
}