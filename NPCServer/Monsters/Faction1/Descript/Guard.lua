function DoConversation(server, player, faction, message)
	if message.find("�ȳ��ϼ���") != nil then
		answer = "������ ��迡�� �̻� �����ϴ�!";	
	end		
	SYSTEM_Send_Message(server, player, answer);
end