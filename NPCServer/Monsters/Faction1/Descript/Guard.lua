function DoConversation(server, player, faction, message)
{
	if(message.find("�ȳ��ϼ���") != nil)
	{
		answer = "������ ��迡�� �̻� �����ϴ�!";	
	}
			
	SYSTEM_Send_Message(server, player, answer);
}