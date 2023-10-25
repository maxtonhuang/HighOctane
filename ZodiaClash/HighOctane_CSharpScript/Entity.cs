using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


public class Entity
{
    protected Entity() { ID = 0; }

    internal Entity(ulong id)
    {
        ID = id;
    }

    public readonly ulong ID;

    public Vector3 Translation
    {
        get
        {
            //InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
            //return result;
            return new Vector3();
        }
        set
        {
            //InternalCalls.TransformComponent_SetTranslation(ID, ref value);
        }
    }
}

