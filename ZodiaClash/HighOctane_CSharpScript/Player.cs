using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using System.Threading.Tasks;

namespace Players
{
    internal class Player : MonoBehaviour
    {
        public override void Start()
        {
            Console.WriteLine("Player OnCreate" + ID);
            //Console.WriteLine("Player OnCreate");

        }

        public override void Update()
        {
            float speed = 1.0f;
            Vector2 translation = Translation;
            translation.X += Input.GetAxisHorizontal() * speed;
            translation.Y += Input.GetAxisVertical() * speed;


            Vector2 force = Force;
            //force.X += Input.GetAxisHorizontal() * speed;
            //force.Y += Input.GetAxisVertical() * speed;

            Translation = translation;
            Force = force;
            //Console.WriteLine("Force is " + Force.X + ", " + Force.Y);
            Console.WriteLine("Force:" + force.X + ", " + force.Y);

            if (Input.GetAxisHorizontal() == 1)
            {
                //Debug.Log("Right");
                Console.WriteLine("Right");
            }
            else if (Input.GetAxisHorizontal() == -1)
            {
                //Debug.Log("Left");
                Console.WriteLine("Left");
            }

            if (Input.GetAxisVertical() == 1)
            {
                //Debug.Log("Up");
                Console.WriteLine("Up");
            }

            else if (Input.GetAxisVertical() == -1)
            {
                //Debug.Log("Down");
                Console.WriteLine("Down");
            }
        }
    }

}
namespace PlayerController
{
    internal class PlayerMovement : MonoBehaviour
    {
        public override void Start()
        {
            Console.WriteLine("PlayerMovement OnCreate");
        }

        public override void Update()
        {
            Console.WriteLine("PlayerMovement OnUpdate");
        }
    }
}

namespace JustTest
{
    internal class Test : MonoBehaviour
    {
        public override void Start()
        {
            Console.WriteLine("Test OnCreate");
        }

        public override void Update()
        {
            Console.WriteLine($"Test OnUpdate");
        }
    }
}