function DoConversation(server, player, faction, message)
	if message.find("�ȳ��ϼ���") != nil then
		answer = "�ȳ��ϼ���. ������ �����Ͻþ� �����ں��� ��ȥ�� �� ���� [���ƿ�����] �ص帮�ڽ��ϴ�."
	elseif message.find("���ƿ�����") != nil then
		answer = "����� ���� ���� �� ������ ���� �ٽ� ������ �� �ֽ��ϴ�."
		SYSTEM_Set_RespawnPosition(server, player)
	else
	end

	SYSTEM_Send_Message(server, player, answer)
end