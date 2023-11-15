using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


internal class AntiClockWiseSpin : MonoBehaviour
{
    public float speed = 1f;
    public int testing = 1;
    public float test = 0.0f;
    public bool yes = false;

    public override void Start()
    {
        Console.WriteLine("AntiClockWiseSpin OnCreate" + ID);
    }

    public override void Update()
    {
        Rotation -= speed * DeltaTime;
    }
}

