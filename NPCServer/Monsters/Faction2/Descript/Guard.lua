function DoConversation(server, npc, player, faction, message)
	if string.find(message, "�ȳ��ϼ���") ~= nil then
		answer = "������ ��迡�� �̻� �����ϴ�!";	
	end		
	SYSTEM_Send_Message(server, npc, player, answer);
end