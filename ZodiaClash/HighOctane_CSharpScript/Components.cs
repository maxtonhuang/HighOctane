//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
#nullable disable

using System.Linq.Expressions;

public abstract class Component 
{
    public Entity Entity { get; internal set; }
}

//public class TransformComponent : Component
//{
    //public Vector2 Translation
    //{
    //    get
    //    {
    //        InternalCalls.EntityGetPosition(Entity.ID, out Vector2 translation);
    //        return translation;
    //    }
    //    set
    //    {
    //        InternalCalls.EntitySetPosition(Entity.ID, ref value);
    //    }
    //}

    //public Vector2 Force
    //{
    //    get
    //    {
    //        InternalCalls.EntityGetForce(Entity.ID, out Vector2 force);
    //        return force;
    //    }

    //    set
    //    {
    //        InternalCalls.EntitySetForce(Entity.ID, ref value);
    //    }
    //}

//}
