using System.Collections.Generic;

public class Message
{
    private MessageType type;
    private List<object> param;
    public MessageType Type
    {
        get
        {
            return type;
        }
    }

    public Message(MessageType _type)
    {
        type = _type;

        param = new List<object>();
    }

    public void Push(object data)
    {
        param.Add(data);
    }

    public object GetParam(int idx)
    {
        return param[idx];
    }

}
