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
	memory.ReadLong(pid, �����ַB, address);
	if (memory.WriteFloat(pid, (address + �����������), speed)) {
		printf("��ʼʧ��");
	}
}

void DnfHelper::RoleSpeed(float speed) {
	ULONG_PTR address;
	memory.ReadLong(pid, �����ַB, address);
	memory.WriteFloat(pid, (address + ��������), speed);
}

void DnfHelper::RoleAttack(float speed) {
	ULONG_PTR address;
	memory.ReadLong(pid, �����ַB, address);
	memory.WriteFloat(pid, (address + ���㱶��), speed);
}

void DnfHelper::RoleInvinc() {
	ULONG_PTR address;
	memory.ReadLong(pid, �����ַB, address);
	memory.WriteFloat(pid, (address + ǿɭ�޵�ƫ��), (float)100);
}

void DnfHelper::RoleSuperArmor() {
	ULONG_PTR address;
	memory.ReadLong(pid, �����ַB, address);
	memory.WriteDword(pid, (address + ����ƫ��), 100);
}

void DnfHelper::BuildIgnor() {
	ULONG_PTR address;
	memory.ReadLong(pid, �����ַB, address);
	memory.WriteDword(pid, (address + ������͸), 0);
}

void DnfHelper::MapIgnor() {
	ULONG_PTR address;
	memory.ReadLong(pid, �����ַB, address);
	memory.WriteDword(pid, (address + ��ͼ��͸), 0);
}

void DnfHelper::SkipAnimation() {
	ULONG_PTR address;
	memory.ReadLong(pid, ������ַ, address);
	memory.WriteDword(pid, (address + ����ƫ��), 1);
}

void DnfHelper::QuickFlip() {
	BYTE arr[] = { 15,133 };
	memory.WriteByteArray(pid, ��ط���, arr, 2);
}

void DnfHelper::AutoPick() {
	BYTE arr[] = { 144,144 };
	memory.WriteByteArray(pid, �Զ�����, arr, 2);
}

void DnfHelper::UnlimitedMoveItem() {
	BYTE arr[] = { 144,144,144,144,144 };
	memory.WriteByteArray(pid, �����ƶ���Ʒ, arr, 5);
}