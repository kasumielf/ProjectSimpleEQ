function DoConversation(server, npc, player, faction, message)
	if string.find(message, "�ȳ��ϼ���") ~= nil then
		answer = "�ȳ��ϼ��� ���� ��ħ�Դϴ�. �ۿ��� ���� �����ںа�����, ���� [����]�� ����ϱ�?"
	elseif string.find(message, "����") ~= nil then
		answer = "�׷�����. �� �ٷ� ��񺴵��� �Ʒ��� ��ȭ�ϰ� ������ ������ ���� �����մϴ�. �׷��� �׷��� Ȥ�� �� ���� [��]�� ���ֽðڽ��ϱ�?";
	elseif string.find(message, "��") ~= nil then 
		answer = "������ ������ ������ �ֽø� �˴ϴ�. [����] �Ͻðڽ��ϱ�?"
	elseif string.find(message, "����") ~= nil then
		answer = "�����ּż� �����մϴ�."
	else
	end
	
	if answer ~= nil then
		SYSTEM_Send_Message(server, npc, player, answer)
	end
end