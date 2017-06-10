using System.Collections.Generic;


public class MessageQueue
{
    private static MessageQueue instance;
    public static MessageQueue getInstance
    {
        get
        {
            if (instance == null)
                instance = new MessageQueue();

            return instance;
        }
    }

    private Queue<Message> messages;
    private MessageQueue()
    {
        messages = new Queue<Message>();
    }
    public bool IsEmpty()
    {
        return messages.Count > 0 ? false : true;
    }

    public void Enqueue(Message data)
    { 
        messages.Enqueue(data);
    }

    public Message Dequeue()
    {
        if (messages.Count > 0)
            return messages.Dequeue();
        else
            return null;
    }
}
