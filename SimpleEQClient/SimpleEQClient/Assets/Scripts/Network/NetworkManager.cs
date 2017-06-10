using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Assets.Scripts.Network;
using Scripts.Utility;
using ClientPacket;
using ServerPacket;
using System.Globalization;

public class NetworkManager : MonoBehaviour
{
    // Use this for initialization
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {

    }

    SocketClass socket = new SocketClass();

    public void Connect(string ip, short port)
    {
        Debug.Log("Socket Connect to" + ip + ":" + port);
        socket.Connect(ip, port);
    }

    public bool isConnected()
    {
        return socket.isConnected();
    }

    public void Disconnect()
    {
        socket.Disconnect();
    }

    public void SetPacketProcessHandler(packetProcess pp)
    {
        socket.SetPacketProcessHandler(pp);
    }

    public void Send(byte[] data)
    {
        socket.SendData(data);
    }

    private void OnApplicationQuit()
    {
        socket.Disconnect();
    }

}
