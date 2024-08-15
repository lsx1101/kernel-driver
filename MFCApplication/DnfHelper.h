#pragma once
#include"MemoryReaderWriter.h"
#include"process.h"
#include"address.h"

class DnfHelper {
public:
	DnfHelper();
	~DnfHelper();
	void TownSpeed(float speed);
	void RoleSpeed(float speed);
	void RoleAttack(float speed);
	void RoleInvinc();
	void RoleSuperArmor();
	void BuildIgnor();
	void MapIgnor();
	void SkipAnimation();
	void QuickFlip();
	void AutoPick();
	void UnlimitedMoveItem();
private:
	ULONG pid;
	CMemoryReaderWriter memory;
};