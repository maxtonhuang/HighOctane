using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public abstract class MonoBehaviour : Entity
{
    virtual public void Start()
    {

    }

    virtual public void Update()
    {

    }
    virtual public void FixedUpdate()
    {

    }

    // Can have a on shut down
    virtual public void ShutDown()
    {

    }

    // Can have a on triggered, or even a set active?
    virtual public void OnTriggered()
    {

    }

    virtual public void SetActive()
    {

    }

}