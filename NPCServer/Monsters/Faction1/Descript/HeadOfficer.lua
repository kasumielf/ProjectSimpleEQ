function DoConversation(server, layer, faction, message)
{
	if(message.find("안녕하세요") != nil)
	{
		answer = "안녕하세요 보아하니 여행자분이신 것 같군요. 천천히 있으시다 가세요.";	
	}
	else if(message.find("옆사람") != nil)
	{
		answer = "아, 제 옆에 있는 분이 신경쓰이시나 보군요. 이 분은 마을에서 꽤나 오래 계신 분입니다. 이 분쟁의 역사를 잘 기억하시죠.";	
	}
	else if(message.find("분쟁") != nil)
	{
		answer = "저 도시와는 원래부터 적대적이진 않고 서로 교류를 하던 사이였습니다. 어떤 사건이 생기기 전 까진요.";
	}
	else if(message.find("어떤 사건") != nil)
	{
		answer = "죄송한 말씀이지만 오늘 초면이신 분께 그런 얘기를 드리긴 좀 그렇네요. 죄송합니다.";
	}
	else
	{
	}
			
	SYSTEM_Send_Message(server, player, answer);
}