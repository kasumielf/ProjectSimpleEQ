using System.Collections.Generic;

public class Message
{
    private MessageType type;
    private List<string> param;
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

        param = new List<string>();
    }

    public void Push(string data)
    {
        param.Add(data);
    }

    public string GetParam(int idx)
    {
        return param[idx];
    }

}
