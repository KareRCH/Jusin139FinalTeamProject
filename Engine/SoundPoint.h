#pragma once

#include "Base.h"
#include "Utility/LogicDeviceBasic.h"
#include "Sound_Manager.h"

#define SOUNDPOINT_DEBUG 1







BEGIN(Engine)

struct TSoundPointDesc
{
	vector<string>	GroupNames;
	vector<string>	SoundNames;
	ESoundGroup		eSoundGroup;
	FMOD_MODE		eRollOffMode;
	_float			fVolume;
	_float			f3DMin;
	_float			f3DMax;
	_float3			vPosition;
};

/// <summary>
/// �ʿ� ��ġ�ϴ� ������ ���带 �����ϱ� ���� Ŭ����
/// </summary>
class ENGINE_DLL CSoundPoint  final : public CBase
{
private:
	CSoundPoint();
	CSoundPoint(const CSoundPoint& rhs);
	virtual ~CSoundPoint() = default;

public:
	HRESULT Initialize(const string& strName, const TSoundPointDesc& SoundPointDesc);
	void Tick(_cref_time fTimeDelta);
	HRESULT Render();
#ifdef _DEBUG
	HRESULT Render_Debug();
#endif // _DEBUG






#pragma region ����
public:
	void					Update_SoundOwner();
	void					Set_Owner(weak_ptr<CGameObject> _pOwner);
	weak_ptr<CGameObject>	Get_Owner();
	_bool					IsOwnerValid() const { return !m_pOwner.expired(); }

private:
	weak_ptr<CGameObject>	m_pOwner;
#pragma endregion






#pragma region Common
public:
	// ����Ʈ�� AABB�� ������� �Լ�
	void Invalidate_SoundPoint();
#pragma endregion







#pragma region ������
public:
	void Add_SoundData(const string& strGroupName, const string& strSoundName);
	void Set_SoundExtDatas(ESoundGroup eSoundGroup = ESoundGroup::Environment, _float fVolume = 1.f);

	const vector<string>& Get_GroupNames() const { return m_SoundPointDesc.GroupNames; }
	const vector<string>& Get_SoundNames() const { return m_SoundPointDesc.SoundNames; }
	void Clear_SoundNameDatas();

	void Set_Name(string strName) { m_strName = strName; }
	string Get_Name() const { return m_strName; }

	void Set_Position(_float3 vPos);
	const _float3 Get_Position();

	void Set_Volume(_float fVolume) { m_SoundPointDesc.fVolume = fVolume; }
	const _float Get_Volume() const { return m_SoundPointDesc.fVolume; }

	void Set_SoundGroup(ESoundGroup eSoundGroup) { m_SoundPointDesc.eSoundGroup = eSoundGroup; }
	const ESoundGroup Get_SoundGroup() const { return m_SoundPointDesc.eSoundGroup; }

	const TSoundPointDesc Get_SoundPointDesc();
	void Set_SoundPointDesc(const TSoundPointDesc Desc);

	void Set_Dead() { m_bDead = true; }
	const _bool Get_Dead() const { return m_bDead; }

private:
	_bool				m_bNeedInvalidate_Position = { false };
	_bool				m_bNeedInvalidate_Sound = { false };
	string				m_strName = { "" };

	TSoundPointDesc		m_SoundPointDesc = {};
	FMOD::Channel*		m_pPlayingChannel = { nullptr };			// �ּ� üũ��
	_int				m_iPlayingChannel_Index = { -1 };	
	_float3				m_vRandomRange = {};						// ������ ��ġ
	_bool				m_bDead = { false };						// ���� �� ���
#pragma endregion







#pragma region �����
#ifdef _DEBUG
private:
	_bool										m_bIsCloned = { false };
	BoundingSphere								m_Sphere;	// ���� �׸����
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = { nullptr };
	BasicEffect*								m_pEffect = { nullptr };
	ComPtr<ID3D11InputLayout>					m_pInputLayout = { nullptr };
#endif // _DEBUG  
#pragma endregion



public:
	static shared_ptr<CSoundPoint> Create(const string& strName, const TSoundPointDesc& SoundPointDesc);
	shared_ptr<CSoundPoint> Clone();
	virtual void Free() override;
};

END