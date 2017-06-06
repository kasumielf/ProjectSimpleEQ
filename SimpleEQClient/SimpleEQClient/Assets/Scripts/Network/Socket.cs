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
        private Int32 recvPacketSize;
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
            object obj = ar.AsyncState;

            Int32 io_size = this.client.EndReceive(ar);
            Int32 packet_size = recvPacketSize;
            Int32 prev_packet_size = io_size + prevRecvPacketSize;

            byte[] packet = new byte[256];

            while (io_size != 0)
            {
                if (requiredPacketSize == 0)
                    requiredPacketSize = recvBuffer[2];

                if (io_size + recvPacketSize >= prevRecvPacketSize)
                {
                    Array.Copy(recvBuffer, 0, packetBuffer, recvPacketSize, requiredPacketSize - recvPacketSize);
                    packetProcessHandler(packetBuffer);

                    io_size -= requiredPacketSize - recvPacketSize;
                    requiredPacketSize = 0;
                    recvPacketSize = 0;
                }
                else
                {
                    Array.Copy(recvBuffer, 0, packetBuffer, recvPacketSize, io_size);
                    recvPacketSize += io_size;
                    io_size = 0;
                }
            }

            client.BeginReceive(this.recvBuffer, 0, this.recvBuffer.Length, SocketFlags.None, new AsyncCallback(recvHandler), this);
        }

        public void SendData(byte[] data)
        {
            requiredSendByte = data.Length;
            Array.Copy(data, sendBuffer, requiredSendByte);

            try
            {
                client.BeginSend(sendBuffer, 0, requiredSendByte, SocketFlags.None, Send, this);
            }
            catch (Exception e)
            {
                Debug.Log(e.Message);
            }

        }

        public void Send(IAsyncResult ar)
        {
            sendByte = client.EndSend(ar);
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
