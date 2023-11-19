using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


internal class AntiClockWiseSpin : MonoBehaviour
{
    public float speed = 1f;

    public override void Start()
    {
        Console.WriteLine("AntiClockWiseSpin OnCreate" + ID);
    }

    public override void Update()
    {
        Rotation -= speed * DeltaTime;

        Console.WriteLine("AntiClockWiseSpin OnUpdate" + speed);
    }
}

