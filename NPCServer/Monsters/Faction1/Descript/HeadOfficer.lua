function DoConversation(server, layer, faction, message)
	if message.find("�ȳ��ϼ���") != nil then
		answer = "�ȳ��ϼ��� �����ϴ� �����ں��̽� �� ������. õõ�� �����ô� ������.";	
	elseif message.find("�����") != nil then
		answer = "��, �� ���� �ִ� ���� �Ű澲�̽ó� ������. �� ���� �������� �ϳ� ���� ��� ���Դϴ�. �� ������ ���縦 �� ����Ͻ���."
	elseif message.find("����") != nil then
		answer = "�� ���ÿʹ� �������� ���������� �ʰ� ���� ������ �ϴ� ���̿����ϴ�. � ����� ����� �� ������."
	elseif message.find("� ���") != nil then
		answer = "�˼��� ���������� ���� �ʸ��̽� �в� �׷� ��⸦ �帮�� �� �׷��׿�. �˼��մϴ�."
	else
	end
	
	SYSTEM_Send_Message(server, player, answer)
end