function DoConversation(server, npc, player, faction, message)
	if string.find(message, "�ȳ��ϼ���") ~= nil then
		answer = "�ȳ��ϼ��� �����ϴ� �����ں��̽� �� ������. õõ�� �����ô� ������.";	
	elseif string.find(message, "�����") ~= nil then
		answer = "��, �� ���� �ִ� ���� �Ű澲�̽ó� ������. �� ���� �������� �ϳ� ���� ��� ���Դϴ�. �� ������ ���縦 �� ����Ͻ���."
	elseif string.find(message, "����") ~= nil then
		answer = "�� ���ÿʹ� �������� ���������� �ʰ� ���� ������ �ϴ� ���̿����ϴ�. � ����� ����� �� ������."
	elseif string.find(message, "� ���") ~= nil then
		answer = "�˼��� ���������� ���� �ʸ��̽� �в� �׷� ��⸦ �帮�� �� �׷��׿�. �˼��մϴ�."
	else
	end
	
	SYSTEM_Send_Message(server, npc, player, answer)
end