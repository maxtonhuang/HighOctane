﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public struct Vector2
{
    public float X, Y;

    public static Vector2 Zero => new Vector2(0.0f);

    public Vector2(float scalar)
    {
        X = scalar;
        Y = scalar;
    }

    public Vector2(float x, float y)
    {
        X = x;
        Y = y;
    }

    public static Vector2 operator +(Vector2 a, Vector2 b)
    {
        return new Vector2(a.X + b.X, a.Y + b.Y);
    }

    public static Vector2 operator *(Vector2 vector, float scalar)
    {
        return new Vector2(vector.X * scalar, vector.Y * scalar);
    }

    public float LengthSquared()
    {
        return X * X + Y * Y;
    }

    public float Length()
    {
        return (float)Math.Sqrt(LengthSquared());
    }

    public float Normalize()
    {
        float length = Length();
        X /= length;
        Y /= length;
        return length;
    }
}
public struct Vector3
{
    public float X, Y, Z;

    public static Vector3 Zero => new Vector3(0.0f);

    public Vector3(float scalar)
    {
        X = scalar;
        Y = scalar;
        Z = scalar;
    }

    public Vector3(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    public Vector3(Vector2 xy, float z)
    {
        X = xy.X;
        Y = xy.Y;
        Z = z;
    }

    public Vector2 XY
    {
        get => new Vector2(X, Y);
        set
        {
            X = value.X;
            Y = value.Y;
        }
    }

    public static Vector3 operator +(Vector3 a, Vector3 b)
    {
        return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
    }

    public static Vector3 operator *(Vector3 vector, float scalar)
    {
        return new Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
    }

}

public struct Vector4
{
    public float X, Y, Z, W;

    public static Vector4 Zero => new Vector4(0.0f);

    public Vector4(float scalar)
    {
        X = scalar;
        Y = scalar;
        Z = scalar;
        W = scalar;
    }

    public Vector4(float x, float y, float z, float w)
    {
        X = x;
        Y = y;
        Z = z;
        W = w;
    }

    public Vector4(Vector3 xyz, float w)
    {
        X = xyz.X;
        Y = xyz.Y;
        Z = xyz.Z;
        W = w;
    }

    public Vector2 XY
    {
        get => new Vector2(X, Y);
        set
        {
            X = value.X;
            Y = value.Y;
        }
    }

    public Vector3 XYZ
    {
        get => new Vector3(X, Y, Z);
        set
        {
            X = value.X;
            Y = value.Y;
            Z = value.Z;
        }
    }

    public static Vector4 operator +(Vector4 a, Vector4 b)
    {
        return new Vector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W);
    }

    public static Vector4 operator *(Vector4 vector, float scalar)
    {
        return new Vector4(vector.X * scalar, vector.Y * scalar, vector.Z * scalar, vector.W * scalar);
    }

}