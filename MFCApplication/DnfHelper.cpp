#include"pch.h"
#include"DnfHelper.h"
#include"debug.h"

DnfHelper::DnfHelper():pid(0), memory(){
	memory.OpenDriver();
	pid = GetPIDByName(_T("DNF.exe"));
}

DnfHelper::~DnfHelper() {
	memory.CloseDriver();
	pid = 0;
}

void DnfHelper::TownSpeed(float speed) {
	ULONG_PTR address;
	CString msg;
	DebugOutput(_T("pid=%d"), pid);
	memory.ReadLong(pid, 人物基址B, address);
	if (memory.WriteFloat(pid, (address + 浮点城镇移速), speed)) {
		printf("开始失败");
	}
}

void DnfHelper::RoleSpeed(float speed) {
	ULONG_PTR address;
	memory.ReadLong(pid, 人物基址B, address);
	memory.WriteFloat(pid, (address + 浮点移速), speed);
}

void DnfHelper::RoleAttack(float speed) {
	ULONG_PTR address;
	memory.ReadLong(pid, 人物基址B, address);
	memory.WriteFloat(pid, (address + 浮点倍攻), speed);
}

void DnfHelper::RoleInvinc() {
	ULONG_PTR address;
	memory.ReadLong(pid, 人物基址B, address);
	memory.WriteFloat(pid, (address + 强森无敌偏移), (float)100);
}

void DnfHelper::RoleSuperArmor() {
	ULONG_PTR address;
	memory.ReadLong(pid, 人物基址B, address);
	memory.WriteDword(pid, (address + 霸体偏移), 100);
}

void DnfHelper::BuildIgnor() {
	ULONG_PTR address;
	memory.ReadLong(pid, 人物基址B, address);
	memory.WriteDword(pid, (address + 建筑穿透), 0);
}

void DnfHelper::MapIgnor() {
	ULONG_PTR address;
	memory.ReadLong(pid, 人物基址B, address);
	memory.WriteDword(pid, (address + 地图穿透), 0);
}

void DnfHelper::SkipAnimation() {
	ULONG_PTR address;
	memory.ReadLong(pid, 动画基址, address);
	memory.WriteDword(pid, (address + 动画偏移), 1);
}

void DnfHelper::QuickFlip() {
	BYTE arr[] = { 15,133 };
	memory.WriteByteArray(pid, 秒关翻牌, arr, 2);
}

void DnfHelper::AutoPick() {
	BYTE arr[] = { 144,144 };
	memory.WriteByteArray(pid, 自动捡物, arr, 2);
}

void DnfHelper::UnlimitedMoveItem() {
	BYTE arr[] = { 144,144,144,144,144 };
	memory.WriteByteArray(pid, 无限移动物品, arr, 5);
}