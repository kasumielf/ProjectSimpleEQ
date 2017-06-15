function DoConversation(server, player, faction, message)
{
	if(message.find("안녕하세요") != nil)
	{
		answer = "안녕하세요 좋은 아침입니다. 밖에서 오신 여행자분같은데, 밖의 [상태]는 어떻습니까?";	
	}
	else if(message.find("상태가 안좋습니다") != nil)
	{
		answer = "그렇군요. 요 근래 경비병들의 훈련을 강화하고 있지만 아직은 많이 부족합니다. 그래서 그런데 혹시 한 가지 [일]을 해주시겠습니까?";	
	}
	else if(message.find("일") != nil)
	{
		answer = "적들을 적당히 소탕해 주시면 됩니다. [수락] 하시겠습니까?";
	}
	else if(message.find("수락") != nil)
	{
		answer = "도와주셔서 감사합니다.";
	}
	else
	{
	}
			
	SYSTEM_Send_Message(server, player, answer);
}