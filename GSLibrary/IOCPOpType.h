#pragma once

enum class IOCPOpType
{
	OpSend,
	OpRecv,
	OpPlayerAttack,
	OpPlayerMove,
	OpPlayerUpdate,
	OpPlayerDBSave,
	OpNPCAttack,
	OpNPCUpdate,
};