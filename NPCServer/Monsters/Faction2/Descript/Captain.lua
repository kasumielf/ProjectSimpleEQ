function DoConversation(server, player, faction, message)
	if message.find("�ȳ��ϼ���") != nil then
		answer = "�ȳ��ϼ��� ���� ��ħ�Դϴ�. �ۿ��� ���� �����ںа�����, ���� [����]�� ����ϱ�?"
	elseif message.find("���°� �������ϴ�") != nil then
		answer = "�׷�����. �� �ٷ� ��񺴵��� �Ʒ��� ��ȭ�ϰ� ������ ������ ���� �����մϴ�. �׷��� �׷��� Ȥ�� �� ���� [��]�� ���ֽðڽ��ϱ�?";
	elseif message.find("��") != nil then 
		answer = "������ ������ ������ �ֽø� �˴ϴ�. [����] �Ͻðڽ��ϱ�?"
	elseif message.find("����") != nil then
		answer = "�����ּż� �����մϴ�."
	else
	end
	
	SYSTEM_Send_Message(server, player, answer)
end