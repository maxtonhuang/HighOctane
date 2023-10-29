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
            float speed = 1.0f;
            Vector2 translation = Translation;
            translation.X += Input.GetAxisHorizontal() * speed;
            translation.Y += Input.GetAxisVertical() * speed;
            Translation = translation;


            float rotationSpeed = 0.01f;
            float rotation = Rotation;
            

            if (Input.GetKeyDown(INFO.KEY_E))
            {
                rotation += rotationSpeed;
            }
            else if (Input.GetKeyDown(INFO.KEY_Q))
            {
                rotation -= rotationSpeed;
            }

            Rotation = rotation;
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
            float speed = 0.001f;
            float scale = Scale;
            scale += Input.GetAxisHorizontal() * speed;
            Scale = scale;
        }
    }
}