using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using System.Threading.Tasks;

namespace PlayerController
{
    internal class PlayerMovement : MonoBehaviour
    {

        public float speed = 0.0f;
        public float rotationSpeed = 0.01f;

        public override void Start()
        {
            Console.WriteLine("PlayerMovement OnCreate");
        }

        public override void Update()
        {
            Vector2 translation = Position;
            translation.X += Input.GetAxisHorizontal() * speed;
            translation.Y += Input.GetAxisVertical() * speed;
            Position = translation;

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