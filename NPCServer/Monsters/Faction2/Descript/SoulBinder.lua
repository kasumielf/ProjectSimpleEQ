function DoConversation(server, npc, player, faction, message)
	if string.find(message, "�ȳ��ϼ���") ~= nil then
		answer = "�ȳ��ϼ���. ������ �����Ͻþ� �����ں��� ��ȥ�� �� ���� [���ƿ�����] �ص帮�ڽ��ϴ�."
	elseif string.find(message, "���ƿ�����") ~= nil then
		answer = "����� ���� ���� �� ������ ���� �ٽ� ������ �� �ֽ��ϴ�."
		SYSTEM_Set_RespawnPosition(server, player)
	else
	end

	SYSTEM_Send_Message(server, npc, player, answer)
end