function DoConversation(server, player, faction, message)
{
	if(message.find("안녕하세요") != nil)
	{
		answer = "오늘의 경계에는 이상 없습니다!";	
	}
			
	SYSTEM_Send_Message(server, player, answer);
}