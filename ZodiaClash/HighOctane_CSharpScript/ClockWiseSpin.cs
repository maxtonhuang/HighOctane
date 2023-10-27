using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

internal class ClockWiseSpin : MonoBehaviour
{
    public override void Start()
    {
        Console.WriteLine("ClockWiseSpin OnCreate" + ID);

    }

    public override void Update()
    {
        float speed = 1.0f;
        float rotation = Rotation;
        rotation += speed;
        Rotation = rotation;
    }
}

