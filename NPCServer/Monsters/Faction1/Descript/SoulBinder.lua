function DoConversation(server, npc, player, faction, message)
	if string.find(message, "안녕하세요") ~= nil then
		answer = "안녕하세요. 저에게 말씀하시어 여행자분의 영혼을 이 곳에 [돌아오도록] 해드리겠습니다."
	elseif string.find(message, "돌아오도록") ~= nil then
		answer = "당신은 이제 부터 이 곳에서 모든걸 다시 시작할 수 있습니다."
		SYSTEM_Set_RespawnPosition(server, player)
	else
	end

	SYSTEM_Send_Message(server, npc, player, answer)
end