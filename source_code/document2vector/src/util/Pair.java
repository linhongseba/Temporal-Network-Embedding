package util;

import java.io.Serializable;

public class Pair
    implements Serializable
{
    private Object key;
    private Object value;

    public Pair(Object key, Object value)
    {
        this.key = key;
        this.value = value;
    }

    public Object getKey()
    {
        return key;
    }

    public void setKey(Object key)
    {
        this.key = key;
    }

    public Object getValue()
    {
        return value;
    }

    public void setValue(Object value)
    {
        this.value = value;
    }
}