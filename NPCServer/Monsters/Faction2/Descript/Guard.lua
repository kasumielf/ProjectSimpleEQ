function DoConversation(server, player, faction, message)
	if message.find("안녕하세요") != nil then
		answer = "오늘의 경계에는 이상 없습니다!";	
	end		
	SYSTEM_Send_Message(server, player, answer);
end