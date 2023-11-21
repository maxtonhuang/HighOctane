using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;


internal class PlayerTest : MonoBehaviour
{
    public float speed = 1f;

    public float scale = 1f;
    public float testtime = 0f;

    public bool yes = false;
    public float rotation = 0f;

    public Vector2 position = new Vector2(0f, 0f);

    public override void Start()
    {
    }

    public override void Update()
    {
        testtime += DeltaTime;
        // Maybe this is something you need to do
        Translation = position;

        Rotation = rotation;

        Scale = scale;

    }
}

