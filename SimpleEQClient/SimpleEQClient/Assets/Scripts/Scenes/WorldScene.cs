using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using ClientPacket;
using Scripts.Utility;
using System;
using System.Text;
using Objects;

public class WorldScene : MonoBehaviour {

    GameObject BaseEnemyPrefab;

    public Text myX;
    public Text myY;

    public Text myName;
    public Text myLevel;
    public Text myCurrHp;
    public Text myMaxHp;
    public Text myExp;
    public Text myBaseDmg;

    private Player myPlayer;
    private Dictionary<uint, GameObject> mobs;
    private List<Objects.Object> mySightList;
    public GameObject myPlayerObject;

    MOVE movePacket = new MOVE();

    public NetworkManager nm;

    // Use this for initialization
    void Start()
    {
        BaseEnemyPrefab = Resources.Load("Prefabs/BaseEnemy") as GameObject;
    }

    private void Awake()
    {
        nm.SetPacketProcessHandler(packetProcess);
        StartCoroutine(checkMessageQueue());
    }

    private void ProcessMessage(Message msg)
    {
        switch (msg.Type)
        {
            case MessageType.CONNECT_TO_WORLD:
                {
                    string ip = msg.GetParam(0);
                    short port = short.Parse(msg.GetParam(1));
                    uint user_uid = uint.Parse(msg.GetParam(2));
                    nm.Connect(ip, port);

                    while (nm.isConnected() != true) ;

                    Request_Enter_GameWorld packet = new Request_Enter_GameWorld();
                    packet.user_uid = user_uid;

                    nm.Send(Utility.ToByteArray(packet));

                    break;
                }
            case MessageType.UPDATE_USER_POSITION:
                {
                    myPlayerObject.transform.position = new Vector3(myPlayer.x * 4, 0.1f, myPlayer.y * 4);

                    break;
                }
            case MessageType.ATTACK_NPC:
                {
                    uint attacking_id = uint.Parse(msg.GetParam(0));
                    uint attacking_damage = uint.Parse(msg.GetParam(1));

                    if (attacking_id == 0)
                        Debug.Log("No attack target in sight");
                    else
                        Debug.Log("Attack to " + msg.GetParam(0) + "/" + msg.GetParam(1));
                    break;
                }
            case MessageType.CREATE_NPC:
                {
                    NonPlayer npc = new NonPlayer();


                    break;
                }
        }
    }

    private IEnumerator checkMessageQueue()
    {
        while (true)
        {
            if(MessageQueue.getInstance.IsEmpty() == false)
            {
                Message msg = MessageQueue.getInstance.Dequeue();

                if (msg != null)
                    ProcessMessage(msg);
            }

            yield return new WaitForSeconds(1);
        }

        yield return 0;
    }
	
	// Update is called once per frame
	void Update () {
        UpdateMyPlayerUI();

        if(Input.GetKeyDown(KeyCode.W))
        {
            myPlayerObject.transform.position += new Vector3(0.0f, 0.0f, 4.0f);
            movePacket.DIR = 2;
            nm.Send(Utility.ToByteArray(movePacket));
        }
        else if(Input.GetKeyDown(KeyCode.A))
        {
            myPlayerObject.transform.position += new Vector3(-4.0f, 0.0f, 0.0f);
            movePacket.DIR = 8;
            nm.Send(Utility.ToByteArray(movePacket));
        }
        else if(Input.GetKeyDown(KeyCode.S))
        {
            myPlayerObject.transform.position += new Vector3(0.0f, 0.0f, -4.0f);
            movePacket.DIR = 6;
            nm.Send(Utility.ToByteArray(movePacket));
        }
        else if(Input.GetKeyDown(KeyCode.D))
        {
            myPlayerObject.transform.position += new Vector3(4.0f, 0.0f, 0.0f);
            movePacket.DIR = 4;
            nm.Send(Utility.ToByteArray(movePacket));
        }
        else if(Input.GetKeyDown(KeyCode.Q))
        {
            myPlayerObject.transform.Rotate(Vector3.up, -90);
        }
        else if(Input.GetKeyDown(KeyCode.E))
        {
            myPlayerObject.transform.Rotate(Vector3.up, 90);
        }
        else if(Input.GetKeyDown(KeyCode.F))
        {
            if (myPlayer.state == ObjectState.Idle)
            {
                myPlayer.state = ObjectState.Battle;
                Debug.Log("Attack Start!");
            }
            else
            {
                myPlayer.state = ObjectState.Idle;
                Debug.Log("Attack End!");
            }

            ATTACK packet = new ATTACK();
            nm.Send(Utility.ToByteArray(packet));
        }
        else
        {

        }
    }

    private void UpdateMyPlayerUI()
    {
        if(myPlayer != null)
        {
            myName.text = myPlayer.name;
            myLevel.text = myPlayer.level.ToString();
            myCurrHp.text = myPlayer.max_hp.ToString();
            myMaxHp.text = myPlayer.max_hp.ToString();
            myExp.text = myPlayer.exp.ToString();
            myBaseDmg.text = myPlayer.base_damage.ToString();
            myX.text = myPlayer.x.ToString();
            myY.text = myPlayer.y.ToString();
        }
    }
    private void OnApplicationQuit()
    {
        nm.Disconnect();
    }

    private void packetProcess(byte[] data)
    {
        ServerPacket.PacketId id = (ServerPacket.PacketId)data[0];

        switch (id)
        {
            case ServerPacket.PacketId.ID_LOGIN_OK:
                {
                    ServerPacket.LOGIN_OK res = (ServerPacket.LOGIN_OK)Utility.ByteArrayToObject(data, typeof(ServerPacket.LOGIN_OK));
                    myPlayer = new Player();
                    mySightList = new List<Objects.Object>();

                    myPlayer.id = res.id;
                    myPlayer.name = new string(res.username.ToCharArray());
                    myPlayer.exp = res.exp;
                    myPlayer.curr_hp = res.hp;
                    myPlayer.max_hp = res.max_hp;
                    myPlayer.level = res.level;
                    myPlayer.x = res.x_pos;
                    myPlayer.y = res.y_pos;
                    myPlayer.base_damage = res.base_damage;

                    Message msg = new Message(MessageType.UPDATE_USER_POSITION);
                    MessageQueue.getInstance.Enqueue(msg);
                    break;
                }
            case ServerPacket.PacketId.ID_Notify_Player_Attack_NPC:
                {
                    ServerPacket.Notifty_Player_Attack_NPC res = (ServerPacket.Notifty_Player_Attack_NPC)Utility.ByteArrayToObject(data, typeof(ServerPacket.Notifty_Player_Attack_NPC));

                    Message msg = new Message(MessageType.ATTACK_NPC);
                    msg.Push(res.npc_id.ToString());
                    msg.Push(res.damage.ToString());

                    MessageQueue.getInstance.Enqueue(msg);
                    break;
                }
            case ServerPacket.PacketId.ID_ADD_OBJECT:
                {
                    ServerPacket.ADD_OBJECT res = (ServerPacket.ADD_OBJECT)Utility.ByteArrayToObject(data, typeof(ServerPacket.ADD_OBJECT));

                    NonPlayer npc = new NonPlayer();

                    npc.id = res.ID;
                    npc.name = res.name;
                    npc.state = ObjectState.Idle;
                    npc.x = res.x;
                    npc.y = res.y;

                    Message msg = new Message(MessageType.CREATE_NPC);
                    msg.Push(npc);
                    MessageQueue.getInstance.Enqueue(msg);
                    break;
                }
        }
    }

 
    public void GameExit()
    {
        StartCoroutine(SocketClose());
    }

    public IEnumerator SocketClose()
    {
        LOGOUT packet = new ClientPacket.LOGOUT();

        if (myPlayer != null)
            packet.user_id = myPlayer.id;

        nm.Send(Utility.ToByteArray(packet));

        yield return new WaitForSeconds(3);

        nm.Disconnect();
        Application.Quit();
    }

}
