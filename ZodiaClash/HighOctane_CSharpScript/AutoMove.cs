using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

internal class AutoMoveX : MonoBehaviour
{
    bool movingRight = true;
    public override void Start()
    {
        Console.WriteLine("AutoMoveX OnCreate" + ID);

    }
    public override void Update()
    {
        float speed = 1.0f;

        Vector2 translation = Position;

        if (movingRight)
        {
            translation.X += speed;

            if (translation.X >= 800)
                movingRight = false;
            // If
        }
        else
        {
            translation.X -= speed;

            if (translation.X <= -800)
                movingRight = true;
        }
        Position = translation;
    }
}

internal class AutoMoveY : MonoBehaviour
{
    bool movingUp = true;
    public override void Start()
    {
        Console.WriteLine("AutoMoveY OnCreate" + ID);

    }
    public override void Update()
    {
        float speed = 1.0f;
        Vector2 translation = Position;


        if (movingUp)
        {
            translation.Y += speed;

            if (translation.Y >= 500)
                movingUp = false;
        }
        else
        {
            translation.Y -= speed;

            if (translation.Y <= -500)
                movingUp = true;
        }
        Position = translation;
    }
}