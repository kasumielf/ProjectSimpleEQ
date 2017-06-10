#pragma once

class Quest
{
private:
	unsigned int quest_id;
	bool cleared;

public:
	bool IsCleared() { return cleared; }
	void SetCleared() { cleared = true; }
};