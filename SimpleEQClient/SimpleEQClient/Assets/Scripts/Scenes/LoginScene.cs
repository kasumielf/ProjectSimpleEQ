using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using ClientPacket;
using Scripts.Utility;
using System;
using System.Text;
using UnityEngine.SceneManagement;

public class LoginScene : MonoBehaviour {

    public InputField username;
    public NetworkManager nm;
    private MessageQueue mq;

    private void ProcessMessage(Message msg)
    {
        switch (msg.Type)
        {
            case MessageType.SCENE_CHANGE_LOGIN_TO_WORLD:
                {
                    Message _msg = new global::Message(MessageType.CONNECT_TO_WORLD);
                    _msg.Push(msg.GetParam(0));
                    _msg.Push(msg.GetParam(1));
                    _msg.Push(msg.GetParam(2));
                    MessageQueue.getInstance.Enqueue(_msg);

                    SceneManager.LoadScene("WorldScene");

                    break;
                }
        }
    }

    private IEnumerator checkMessageQueue()
    {
        while (true)
        {
            if (MessageQueue.getInstance.IsEmpty() == false)
            {
                Message msg = MessageQueue.getInstance.Dequeue();

                if (msg != null)
                    ProcessMessage(msg);
            }

            yield return new WaitForSeconds(1);
        }

        yield return 0;
    }

    private void Awake()
    {
        StartCoroutine(checkMessageQueue());
    }
    // Use this for initialization
    void Start () {

        nm.SetPacketProcessHandler(packetProcess);
	}
	
	// Update is called once per frame
	void Update () {
		
	}
//    SceneManager.LoadScene("WorldScene");

    private void packetProcess(byte[] data)
    {
        ServerPacket.PacketId id = (ServerPacket.PacketId)data[0];

        switch (id)
        {
            case ServerPacket.PacketId.ID_LOGIN_OK:
                {
                    Debug.Log("LOGIN OK!");
                }
            break;

            case ServerPacket.PacketId.ID_CONNECT_SERVER:
                {
                    ServerPacket.CONNECT_SERVER res = (ServerPacket.CONNECT_SERVER)Utility.ByteArrayToObject(data, typeof(ServerPacket.CONNECT_SERVER));

                    Message msg = new Message(MessageType.SCENE_CHANGE_LOGIN_TO_WORLD);

                    msg.Push(res.ip);
                    msg.Push(res.port.ToString());
                    msg.Push(res.user_uid.ToString());

                    MessageQueue.getInstance.Enqueue(msg);
                }
                break;
        }
    }

    private void OnApplicationQuit()
    {
        nm.Disconnect();
    }

    public void Login()
    {
        if(nm.isConnected() == false)
        {
            nm.Connect("127.0.0.1", 4000);
        }

        string uname = username.text;

        if(uname.Equals("") == false)
        {
            LOGIN login_packet = new LOGIN();

            login_packet.ID_STR = uname;

            nm.Send(Utility.ToByteArray(login_packet));
        }
    }
}
