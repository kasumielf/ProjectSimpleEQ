﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using ClientPacket;
using Scripts.Utility;
using System;
using System.Text;
using Objects;

public class WorldScene : MonoBehaviour {
    public Text myX;
    public Text myY;

    public Text myName;
    public Text myLevel;
    public Text myCurrHp;
    public Text myMaxHp;
    public Text myExp;
    public Text myBaseDmg;

    public BaseEnemy baseEnemyPrefab;

    public Text damageLog;

    private Player myPlayer;
    private Dictionary<uint, Player> players;
    private Dictionary<uint, BaseEnemy> mobs;
    private List<Objects.Object> mySightList;
    private Dictionary<uint, GameObject> playerObject;
    public GameObject myPlayerObject;


    MOVE movePacket = new MOVE();

    public NetworkManager nm;

    // Use this for initialization
    void Start()
    {
        baseEnemyPrefab = Resources.Load("Prefabs/BaseEnemy", typeof(BaseEnemy)) as BaseEnemy;
        mobs = new Dictionary<uint, BaseEnemy>();
        playerObject = new Dictionary<uint, GameObject>();
        players = new Dictionary<uint, Player>();
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
                    string ip = msg.GetParam(0).ToString();
                    short port = Convert.ToInt16(msg.GetParam(1));
                    uint user_uid = Convert.ToUInt32(msg.GetParam(2));
                    nm.Connect(ip, port);

                    while (nm.isConnected() != true) ;

                    Request_Enter_GameWorld packet = new Request_Enter_GameWorld();
                    packet.user_uid = user_uid;

                    nm.Send(Utility.ToByteArray(packet));

                    break;
                }
            case MessageType.UPDATE_USER_POSITION:
                {
                    uint user_uid = Convert.ToUInt32(msg.GetParam(0));
                    ushort x = Convert.ToUInt16(msg.GetParam(1));
                    ushort y = Convert.ToUInt16(msg.GetParam(2));

                    if(user_uid == myPlayer.id)
                    {
                        myPlayer.x = x;
                        myPlayer.y = y;
                        myPlayerObject.transform.position = new Vector3(x, 0.1f, y);
                    }
                    else
                    {
                        players[user_uid].x = x;
                        players[user_uid].y = y;
                        playerObject[user_uid].transform.position = new Vector3(x, 0.1f, y);
                    }
                    break;
                }
            case MessageType.ATTACK_NPC:
                {
                    uint attacking_id = (uint)msg.GetParam(0);
                    uint attacking_damage = (ushort)msg.GetParam(1);

                    if (attacking_id == 0)
                        InputTextLine("공격 가능 범위에 아무도 없습니다!");
                    break;
                }
            case MessageType.CREATE_NPC:
                {
                    NonPlayer npc = msg.GetParam(0) as NonPlayer;

                    Debug.Log(npc.name);
                    if(mobs.ContainsKey(npc.id) == false)
                    {
                        BaseEnemy bm = Instantiate(baseEnemyPrefab, new Vector3(-1, -1, -1), Quaternion.identity);
                        bm.baseObject = npc;

                        mobs.Add(npc.id, bm);
                    }

                    break;
                }
            case MessageType.PLAYER_DAMAGED:
                {
                    uint npc_id = (uint)msg.GetParam(0);
                    uint damage = (ushort)msg.GetParam(1);

                    InputTextLine(String.Format("{0}에게 공격을 받아 {1} 대미지를 입었습니다!", mobs[npc_id].baseObject.name, damage));

                    myPlayer.curr_hp -= damage;
                    break;
                }
                
            case MessageType.REMOVE_OBJECT:
                {
                    uint id = Convert.ToUInt32(msg.GetParam(0));

                    if (mobs.ContainsKey(id) == true)
                    {
                        Destroy(mobs[id].gameObject);
                        mobs.Remove(id);
                    }
                    break;
                }

            case MessageType.DAMAGED_NPC:
                {
                    uint id = Convert.ToUInt32(msg.GetParam(0));
                    ushort hp = Convert.ToUInt16(msg.GetParam(1));
                    ushort dmg = Convert.ToUInt16(msg.GetParam(2));
                    InputTextLine(String.Format("{0}를 공격해서 {1} 대미지를 주었습니다.남은 HP : {2}", mobs[id].baseObject.name, dmg, hp));
                    break;
                }

            case MessageType.UPDATE_USER_HP:
                {
                    ushort hp = Convert.ToUInt16(msg.GetParam(0));
                    myPlayer.curr_hp = hp;
                    InputTextLine(String.Format("체력을 회복해 {0} HP가 되었습니다.", hp));
                    break;
                }
            case MessageType.UPDATE_USER_BASEINFO:
                {
                    ushort level = Convert.ToUInt16(msg.GetParam(0));
                    ulong exp = Convert.ToUInt64(msg.GetParam(1));
                    ushort hp = Convert.ToUInt16(msg.GetParam(2));
                    ushort maxhp = Convert.ToUInt16(msg.GetParam(3));
                    ushort basedamage = Convert.ToUInt16(msg.GetParam(4));

                    if(myPlayer.level != level)
                    {
                        InputTextLine(String.Format("{0} 레벨에서 {1} 레벨이 되었습니다.", myPlayer.level, level));
                        myPlayer.level = level;
                    }

                    if(myPlayer.exp != exp)
                    {
                        InputTextLine(String.Format("경험치 {0}을 획득했습니다.", exp - myPlayer.exp));
                        myPlayer.exp = exp;
                    }

                    myPlayer.curr_hp = hp;
                    myPlayer.max_hp = hp;
                    myPlayer.base_damage = basedamage;

                    break;
                }

        }
    }

    int line_count = 0;

    private void InputTextLine(String text)
    {
        damageLog.text += (text + System.Environment.NewLine);
        if (line_count++ > 5)
        {
            int index = damageLog.text.IndexOf(System.Environment.NewLine);
            string newtxt = damageLog.text.Substring(index + System.Environment.NewLine.Length);
            damageLog.text = newtxt;
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

            yield return new WaitForEndOfFrame();
        }

        yield return 0;
    }
	
	// Update is called once per frame
	void Update () {

        UpdateMyPlayerUI();

        if(Input.GetKeyDown(KeyCode.W))
        {
            myPlayerObject.transform.position += new Vector3(0.0f, 0.0f, 1.0f);
            movePacket.DIR = 2;
            nm.Send(Utility.ToByteArray(movePacket));
        }
        else if (Input.GetKeyDown(KeyCode.D))
        {
            myPlayerObject.transform.position += new Vector3(1.0f, 0.0f, 0.0f);
            movePacket.DIR = 4;
            nm.Send(Utility.ToByteArray(movePacket));
        }
        else if (Input.GetKeyDown(KeyCode.S))
        {
            myPlayerObject.transform.position += new Vector3(0.0f, 0.0f, -1.0f);
            movePacket.DIR = 6;
            nm.Send(Utility.ToByteArray(movePacket));
        }
        else if (Input.GetKeyDown(KeyCode.A))
        {
            myPlayerObject.transform.position += new Vector3(-1.0f, 0.0f, 0.0f);
            movePacket.DIR = 8;
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
            }
            else
            {
                myPlayer.state = ObjectState.Idle;
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
            myCurrHp.text = myPlayer.curr_hp.ToString();
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

                    Player p = new Player();
                    mySightList = new List<Objects.Object>();

                    p.id = res.id;
                    p.name = new string(res.username.ToCharArray());
                    p.exp = res.exp;
                    p.curr_hp = res.hp;
                    p.max_hp = res.max_hp;
                    p.level = res.level;
                    p.x = res.x_pos;
                    p.y = res.y_pos;
                    p.base_damage = res.base_damage;

                    myPlayer = p;

                    Message msg = new Message(MessageType.UPDATE_USER_POSITION);
                    msg.Push(p.id);
                    msg.Push(p.x);
                    msg.Push(p.y);
                    MessageQueue.getInstance.Enqueue(msg);
                    break;
                }
            case ServerPacket.PacketId.ID_Notify_Player_Attack_NPC:
                {
                    ServerPacket.Notifty_Player_Attack_NPC res = (ServerPacket.Notifty_Player_Attack_NPC)Utility.ByteArrayToObject(data, typeof(ServerPacket.Notifty_Player_Attack_NPC));

                    Message msg = new Message(MessageType.ATTACK_NPC);
                    msg.Push(res.npc_id);
                    msg.Push(res.damage);

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

            case ServerPacket.PacketId.ID_REMOVE_OBJECT:
                {
                    ServerPacket.REMOVE_OBJECT res = (ServerPacket.REMOVE_OBJECT)Utility.ByteArrayToObject(data, typeof(ServerPacket.REMOVE_OBJECT));

                    Message msg = new Message(MessageType.REMOVE_OBJECT);
                    msg.Push(res.ID);
                    MessageQueue.getInstance.Enqueue(msg);
                    break;
                }
            case ServerPacket.PacketId.ID_Notify_NPC_Damaged:
                {
                    ServerPacket.Notify_NPC_Damaged res = (ServerPacket.Notify_NPC_Damaged)Utility.ByteArrayToObject(data, typeof(ServerPacket.Notify_NPC_Damaged));

                    Message msg = new Message(MessageType.DAMAGED_NPC);
                    msg.Push(res.npc_id);
                    msg.Push(res.npc_hp);
                    msg.Push(res.gained_damage);
                    MessageQueue.getInstance.Enqueue(msg);
                    break;
                }
            case ServerPacket.PacketId.ID_Notify_Player_HPRegen:
                {
                    ServerPacket.Notify_Player_HPRegen res = (ServerPacket.Notify_Player_HPRegen)Utility.ByteArrayToObject(data, typeof(ServerPacket.Notify_Player_HPRegen));

                    Message msg = new Message(MessageType.UPDATE_USER_HP);
                    msg.Push(res.curr_hp);
                    MessageQueue.getInstance.Enqueue(msg);
                    break;
                }
            case ServerPacket.PacketId.ID_Notify_NPC_Attack_Player:
                {
                    ServerPacket.Notify_NPC_Attack_Player res = (ServerPacket.Notify_NPC_Attack_Player)Utility.ByteArrayToObject(data, typeof(ServerPacket.Notify_NPC_Attack_Player));
                    Message msg = new Message(MessageType.PLAYER_DAMAGED);
                    msg.Push(res.npc_id);
                    msg.Push(res.damage);
                    MessageQueue.getInstance.Enqueue(msg);
                    break;
                }
            case ServerPacket.PacketId.ID_Notify_Player_Move:
                {
                    ServerPacket.Notify_Player_Move_Position res = (ServerPacket.Notify_Player_Move_Position)Utility.ByteArrayToObject(data, typeof(ServerPacket.Notify_Player_Move_Position));

                    Message msg = new Message(MessageType.UPDATE_USER_POSITION);
                    msg.Push(res.id);
                    msg.Push(res.x);
                    msg.Push(res.y);
                    MessageQueue.getInstance.Enqueue(msg);
                    break;
                }
            case ServerPacket.PacketId.ID_Notify_Player_Info:
                {
                    ServerPacket.Notify_Player_Info res = (ServerPacket.Notify_Player_Info)Utility.ByteArrayToObject(data, typeof(ServerPacket.Notify_Player_Info));
                    Message msg = new Message(MessageType.UPDATE_USER_BASEINFO);
                    msg.Push(res.LEVEL);
                    msg.Push(res.EXP);
                    msg.Push(res.HP);
                    msg.Push(res.max_hp);
                    msg.Push(res.base_damage);
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
