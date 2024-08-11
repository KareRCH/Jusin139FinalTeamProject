#pragma once

#include "Base.h"

#include "Utility/LogicDeviceBasic.h"

#include <mutex>
#include <future>
#include <thread>

#define SOUND_MANAGER_BGM FALSE		// FALSE : ��Ȱ��ȭ, TRUE : Ȱ��ȭ


BEGIN(Engine)

class CGameObject;

const static _int g_iMaxSoundChannel = 1024;


enum class ESoundGroup
{
	BGM,			// BGM
	Environment,	// ȯ���
	UI,				// UI��
	Voice,			// �Ϲ� ��Ҹ���
	Narration,		// �����̼ǿ�
	Effect,			// ����Ʈ��
	End
};

const _char* SoundGroupNames[] = {
	"BGM", "Environment", "UI", "Voice", "Narration", "Effect"
};

enum class ESoundGroupRange
{
	BGM_Start,
	BGM_End = 1,
	Environment_Start,
	Environment_End = 500,
	UI_Start,
	UI_End = 550,
	Narration_Start,
	Narration_End = 610,
	Voice_Start,
	Voice_End = 850,
	Effect_Start,
	Effect_End = 1023
};

class ENGINE_DLL FSoundData final : public CBase
{
	INFO_CLASS(FSoundData, CBase)
private:
	FSoundData() {}
	virtual ~FSoundData() {}

public:
	static shared_ptr<FSoundData> Create()
	{
		DERIVED* pInstance = new DERIVED;

		return BaseMakeShared(pInstance);
	}

	virtual void Free()
	{
		mapSound.clear();
	}

public:
	using SMapSound = _unmap<wstring, FMOD::Sound*>;
	GETSET_EX1(SMapSound, mapSound, MapSound, GET_REF)

private:
	SMapSound mapSound;
};

class FChannelGroupData final : public CBase
{

};


/// <summary>
/// ���� ��� Ŭ����
/// </summary>
class CSound_Manager final : public CBase
{
	DERIVED_CLASS(CBase, CSound_Manager)

private:
	explicit CSound_Manager();
	explicit CSound_Manager(const CSound_Manager& rhs) = delete;
	virtual ~CSound_Manager() = default;

public:
	HRESULT		Initialize(const string& strMainPath);
	void		Tick(_cref_time fTimeDelta);

public:
	static CSound_Manager*	Create(const string& strMainPath);

private:
	virtual void		Free() override;
	


#pragma region ���� �Լ�
public:
	// �Ϲ� ���� ���� ��� (UI ��), ��� ä���� ��ȯ��.
	_int Play_Sound(const wstring& strGroupKey, const wstring& strSoundKey,
		ESoundGroup eSoundGroup, _float fVolume, FMOD::Channel** ppOutChannel = nullptr);
	// 3D ���� ��� (���� �÷��� ���� ��)
	_int Play_3DSound(const wstring& strGroupKey, const wstring& strSoundKey,
		ESoundGroup eSoundGroup, _float fVolume, _float3 vPos, FMOD::Channel** ppOutChannel = nullptr);
	_int Play_3DSound(const wstring& strGroupKey, vector<string>& strSoundKeys,
		ESoundGroup eSoundGroup, shared_ptr<CGameObject> pOwner, _float3 vOffsetPos = _float3(0.f, 0.f, 0.f), _float fVolume = 1.f, FMOD::Channel** ppOutChannel = nullptr);
	
	_int Play_3DSound(vector<string>& strGroupKey, vector<string>& strSoundKeys,
		ESoundGroup eSoundGroup, shared_ptr<CGameObject> pOwner, _float3 vOffsetPos = _float3(0.f, 0.f, 0.f), _float fVolume = 1.f, FMOD::Channel** ppOutChannel = nullptr);

	_int Play_3DSound(vector<string> strGroupKeys, vector<string> strSoundKeys, ESoundGroup eSoundGroup
		, _float3 vPos
		, _float fVolume = 1.f, _float f3DMin = 0.5f, _float f3DMax = 50.f, FMOD_MODE eRollOffMode = FMOD_3D_LINEARSQUAREROLLOFF,
		FMOD::Channel** ppOutChannel = nullptr);

	_bool	IsPlaying_Channel(_int* pChannelIndex, FMOD::Channel** ppChannel);
	void	Sound_Move(_int* pChannelIndex, FMOD::Channel** ppChannel, _float3 vPos);
	void	Sound_StopByChannel(_int* pChannelIndex, FMOD::Channel** ppChannel);

	// ��� ���
	_int Play_BGM(const wstring& strGroupKey, const wstring& strSoundKey, _float fVolume);
	_int Play_BGM_LoopRange(const wstring& strGroupKey, const wstring& strSoundKey,
							_float fVolume, _uint fStart, _uint fEnd);

	void FadeOut_BGM(_float fFadeOutSpeed = 1.f);
	// ������� ����Ī
	void Swap_BGM(_float fBlendSpeed = 1.f, _bool bSwapImmediate = false);

	// �Ϲ� ä�� ���� ����
	void Stop_Sound(_int iChannel);
	// �������, �ʵ�� ����
	void Stop_BGM_Front();
	// �������, ����Ī�� ����
	void Stop_BGM_Back();
	// ������� ��ü ����
	void Stop_BGM_All();

	// ��� ���� ����
	void Stop_All();
	// ä���� ���� ����
	void Set_ChannelVolume(_int eID, float fVolume);
#pragma endregion

private:
	FMOD::Sound* Find_Sound(const wstring& strGroupKey, const wstring& strSoundKey);
	void Get_RangeBySoundGroup(ESoundGroup eSoundGroup, _int& iStart, _int& iEnd);

private:
	_int		m_iActiveBGM_Channel = { 0 };	// Ȱ��ȭ�� (����) BGM ä��
	FGauge		m_fBGM_Blend = { 1.f };			// BGM ����
	_float		m_fBGM_BlendSpeed = { 1.f };	// BGM ���� ���ǵ�

	_bool		m_bIsBGM_FadeOut = { false };	// ���̵� �ƿ� ����.
	FGauge		m_fBGM_FadeOut = { 1.f };		// BGM ���̵� �ƿ�
	_float		m_fBGM_FadeOutSpeed = { 1.f };	// ���̵� �ƿ� �ӵ�

#pragma region �ε� �Լ�
private:
	// ���������� �ε��� ���� ���� ������ ������ �ε��ϴ� �Լ�
	void Load_SoundFile(const wstring& pGroupKey, const string& pPath, FMOD_MODE eFlage);
	// ���� ������ �׷� ������ �񵿱� �ε��� �� ���̴� �Լ�
	void Load_SoundFile_GroupAsync(const wstring& pGroupKey, const string& pPath, FMOD_MODE eFlage);
	// �񵿱� ó���� ��ٸ��� �Լ�
	void Wait_GroupAsync();
	// ���� ���� ���Ϸ� �񵿱� �ε��� �� ���̴� �Լ���
	FMOD_RESULT LoadSoundFile_Async(const string& pPath, const string& pFileName,
		FMOD_RESULT& hResult, FMOD::Sound** pSound, FMOD_MODE eMode);
#pragma endregion



public:
	const _unmap<wstring, shared_ptr<FSoundData>>& CRef_SoundMap() const { return m_mapSound; }
	// �׷��� ��� ���� �̸��� ��ȯ��
	vector<wstring> Provide_GroupSoundNames(const wstring& strGroupKey) const;

private:
	using SSoundDataMap = _unmap<wstring, shared_ptr<FSoundData>>;
	string					m_strPainPath;

	// ���� ���ҽ� ������ ���� ��ü 
	SSoundDataMap			m_mapSound;
	mutex					m_mtxSound;
	vector<future<void>>	m_vecAsyncSoundGroup;
	


public:
	// ä�� �׷쿡 ���� ���� ���� �� ������ ��������
	_float	Get_MasterVolume() const;
	void	Set_MasterVolume(_float fVolume);
	_float	Get_BGMVolume() const;
	void	Set_BGMVolume(_float fVolume);
	_float	Get_VoiceVolume() const;
	void	Set_VoiceVolume(_float fVolume);
	_float	Get_EffectVolume() const;
	void	Set_EffectVolume(_float fVolume);

public:
	// Ŭ�� ������ ��︮���� �ӽ÷� ���⼭ �����Ѵ�.
	void NormalModeDSP();
	void WaitModeDSP();

private:
	enum class EDspMode { Normal, Wait };
	EDspMode		m_eDspMode = EDspMode::Normal;

public:
	using SDspMap = _unmap<string, FMOD::DSP*>;

private:
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD::System*		m_pSystem = { nullptr };

	// ä�� �׷�
	FMOD::ChannelGroup* m_pMasterChanelGroup = { nullptr };			// ������ ä�α׷�
	
	FMOD::ChannelGroup*	m_pChannelGroups[ECast(ESoundGroup::End)];	// ���, ��Ҹ�, ȿ����
	SDspMap				m_pChannelGroupDSPs;						// ������ ���� ������

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD::Channel*		m_pChannels[g_iMaxSoundChannel];			// �� ä�� (256��)
	

public:
	// ����� ���� û���� ������ �����ϴ� �Լ�
	void Set_ListenerAttributeByMatrix(_fmatrix Matrix);
	void Set_ListenerLookLength(_float fLength) { m_ListenerLookLength = fLength; }

private:
	FMOD_3D_ATTRIBUTES		m_ListenerAttribute = {};
	_float					m_ListenerLookLength = { 0.f };
};

END