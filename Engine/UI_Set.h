#pragma once
#include "UIObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI_Set : public CBase // UIObject�ϳ��� �ϳ���
{
public:
	typedef struct tagMotionSet_Data { 
		string								strMotionName; //����� �̸�(Click ��)
		vector<CUIObject::UI_MOTION_DESC>	vecMotionData; //������ ������(Ű������, Pos ��)
		MOTION_SETTING_DESC						Motion_Setting_Data; //��� ���� ������(�෹�̼� ��)
	}UI_FULLMOTION_SET;

private:
	CUI_Set();
	virtual ~CUI_Set() = default;

public:
	HRESULT Initialize_Prototype(string _UISetName);

	void InPut_FullMotion_Data(string _MotionName, string _UIName, 
		vector<CUIObject::UI_MOTION_DESC>& _vecMotionData, MOTION_SETTING_DESC _MotionSetData);
	
	void Delete_Motion_Data(string _MotionName); //��ü�� ������ �ִ� ����� ����� �Լ�
	string Get_UISetName() { return m_strUISetName; }

private:
	string									m_strUISetName = { "" }; //UISet ��ü�� �̸�(��: Command)
	map<string, vector<UI_FULLMOTION_SET>>	m_mapFullMotionSet = {};  //Ű�� : UI ��ü �̸�(��: Menu1)

	_int		m_iCmpUICount = { 0 }; //UISet�� ���� UI ��ü ��
	_int		m_iMotionCount = { 0 }; //UISet�� �ִ� ����� ����
	_int		m_iCurrentMotionIdx = { 0 };

public:
	static CUI_Set* Create(string _UISetName);
	virtual void Free() override;
};

END