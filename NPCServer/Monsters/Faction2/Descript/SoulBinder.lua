function DoConversation(server, player, faction, message)
{
	if(message.find("�ȳ��ϼ���") != nil)
	{
		answer = "�ȳ��ϼ���. ������ �����Ͻþ� �����ں��� ��ȥ�� �� ���� [���ƿ�����] �ص帮�ڽ��ϴ�.";	
	}
	else if(message.find("���ƿ�����") != nil)
	{
		answer = "����� ���� ���� �� ������ ���� �ٽ� ������ �� �ֽ��ϴ�.";	
		SYSTEM_Set_RespawnPosition(server, player);
	}
	else
	{
	}

	SYSTEM_Send_Message(server, player, answer);			
}