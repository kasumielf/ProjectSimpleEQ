function DoConversation(server, player, faction, message)
{
	if(message.find("�ȳ��ϼ���") != nil)
	{
		answer = "�ȳ��ϼ��� ���� ��ħ�Դϴ�. �ۿ��� ���� �����ںа�����, ���� [����]�� ����ϱ�?";	
	}
	else if(message.find("���°� �������ϴ�") != nil)
	{
		answer = "�׷�����. �� �ٷ� ��񺴵��� �Ʒ��� ��ȭ�ϰ� ������ ������ ���� �����մϴ�. �׷��� �׷��� Ȥ�� �� ���� [��]�� ���ֽðڽ��ϱ�?";	
	}
	else if(message.find("��") != nil)
	{
		answer = "������ ������ ������ �ֽø� �˴ϴ�. [����] �Ͻðڽ��ϱ�?";
	}
	else if(message.find("����") != nil)
	{
		answer = "�����ּż� �����մϴ�.";
	}
	else
	{
	}
			
	SYSTEM_Send_Message(server, player, answer);
}