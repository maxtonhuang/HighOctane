﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


internal class AntiClockWiseSpin : MonoBehaviour
{
    public override void Start()
    {
        Console.WriteLine("AntiClockWiseSpin OnCreate" + ID);
    }

    public override void Update()
    {
        float speed = 1f;
        float rotation = Rotation;
        rotation -= speed * DeltaTime;
        Rotation = rotation;
    }
}

