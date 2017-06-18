using System;
using System.Net;
using System.Net.Sockets;
using UnityEngine;
using ClientPacket;
using ServerPacket;
using Scripts.Utility;

public delegate void packetProcess(byte[] data);

namespace Assets.Scripts.Network
{
    class SocketClass
    {
        private byte[] recvBuffer;
        private byte[] packetBuffer;
        //private Int32 recvPacketSize;
        private Int32 prevRecvPacketSize;
        private Int32 requiredPacketSize;

        private byte[] sendBuffer;
        private Int32 sendByte;
        private Int32 requiredSendByte;


        private Socket client;
        private bool connected;
        private AsyncCallback recvHandler;
        private AsyncCallback sendHandler;
        private packetProcess packetProcessHandler;


        public SocketClass()
        {
            connected = false;

            recvHandler = new AsyncCallback(Receive);
            sendHandler = new AsyncCallback(Send);

            recvBuffer = new byte[256];
            sendBuffer = new byte[256];
            packetBuffer = new byte[256];
            prevRecvPacketSize = 0;
        }

        public void SetPacketProcessHandler(packetProcess handle)
        {
            packetProcessHandler = new packetProcess(handle);
        }

        public bool Connect(string ip, int port)
        {
            Debug.Log("try connect");
            try
            {
                client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 10000);
                client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 10000);

                client.Connect(ip, port);

                client.BeginReceive(this.recvBuffer, 0, this.recvBuffer.Length, SocketFlags.None, new AsyncCallback(recvHandler), this);

                connected = true;
                Debug.Log("connected");
            }
            catch (Exception e)
            {
                Debug.Log(e.Message);
                connected = false;
                return false;
            }
            return true;
        }

        public void Receive(IAsyncResult ar)
        {
            try
            {
                int nReadSize = client.EndReceive(ar);

                if (nReadSize != 0)
                {
                    packetProcessHandler(recvBuffer);
                    Array.Clear(recvBuffer, 0, 256);
                }

                client.BeginReceive(this.recvBuffer, 0, this.recvBuffer.Length, SocketFlags.None, new AsyncCallback(recvHandler), this);
            }
            catch (SocketException e)
            {
                Debug.Log(e.Message);
            }
            
        }

        public void SendData(byte[] data)
        {
            int size = data[1];

            //requiredSendByte = data.Length;
            Array.Copy(data, sendBuffer, size);

            try
            {
                client.BeginSend(sendBuffer, 0, size, SocketFlags.None, Send, this);
            }
            catch (Exception e)
            {
                Debug.Log(e.Message);
            }

        }

        public void Send(IAsyncResult ar)
        {
            sendByte = client.EndSend(ar);
            Array.Clear(sendBuffer, 0, sendByte);
        }

        public bool Disconnect()
        {
            try
            {
                if (connected)
                {
                    client.Close();

                    connected = false;

                    return true;
                }
            }
            catch (Exception e)
            {
                return false;
            }

            return false;
        }

        public bool isConnected()
        {
            return connected;
        }
    }
}
