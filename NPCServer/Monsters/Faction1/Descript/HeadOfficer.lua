function DoConversation(server, layer, faction, message)
{
	if(message.find("�ȳ��ϼ���") != nil)
	{
		answer = "�ȳ��ϼ��� �����ϴ� �����ں��̽� �� ������. õõ�� �����ô� ������.";	
	}
	else if(message.find("�����") != nil)
	{
		answer = "��, �� ���� �ִ� ���� �Ű澲�̽ó� ������. �� ���� �������� �ϳ� ���� ��� ���Դϴ�. �� ������ ���縦 �� ����Ͻ���.";	
	}
	else if(message.find("����") != nil)
	{
		answer = "�� ���ÿʹ� �������� ���������� �ʰ� ���� ������ �ϴ� ���̿����ϴ�. � ����� ����� �� ������.";
	}
	else if(message.find("� ���") != nil)
	{
		answer = "�˼��� ���������� ���� �ʸ��̽� �в� �׷� ��⸦ �帮�� �� �׷��׿�. �˼��մϴ�.";
	}
	else
	{
	}
			
	SYSTEM_Send_Message(server, player, answer);
}