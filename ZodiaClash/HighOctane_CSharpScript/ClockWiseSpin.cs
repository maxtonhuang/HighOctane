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
        float speed = 1f;
        float deltaTime = DeltaTime;
        float rotation = Rotation;
        rotation += speed * deltaTime;
        Rotation = rotation;
    }
}

