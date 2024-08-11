//#pragma once
//#include "Model.h"
//#include "Animation_Data.h"
//
///* Ư�� �ִϸ��̼�(���, �ȱ�, �ٱ�, ������, �±�) �� ǥ���ϱ����� �����͵��� ������. */
//
//BEGIN(Engine)
//
//class CAnimation final : public CBase
//{
//private:
//	CAnimation();
//	CAnimation(const CAnimation& rhs);
//	virtual ~CAnimation() = default;
//
//public:
//
//#ifdef _DEBUG
//	HRESULT Initialize(const aiAnimation* pAIAnimation, const CModel::BONES& Bones);
//#endif
//	HRESULT Initialize(ANIMATION_DATA AnimationData);
//	void Invalidate_TransformationMatrix(_bool isLoop, _cref_time fTimeDelta, vector<class CBone*> Bones, CAnimation* pPrevAnimation);
//
//private:
//	_char					m_szName[MAX_PATH] = "";
//	_float					m_fDuration = { 0.0f }; /* �� �ִϸ��̼��� ��ü ����ϱ����� ��ü ����. */
//	_float					m_fTickPerSecond = { 0.f }; /* �ִϸ��̼��� ��� �ӵ� : m_TickPerSecond * fTimeDelta */
//	_float					m_fAnimSpeed = { 1.f };
//	_float					m_fAnimAdjustSpeed = { 1.f };
//	_float					m_fTrackPosition = { 0.f }; /* ���� ����ǰ� �ִ� ��ġ. */
//
//	_uint					m_iTrackFrameIndex = { 0 }; /* ���� ����ǰ� �ִ� ������ �ε���*/
//	_float					m_fCorrectionValue = { 0.039f };
//
//	_bool					m_isFinished = { false };
//
//	vector<_uint>			m_CurrentKeyFrames;
//	_uint					m_iNumChannels = { 0 }; /* �� �ִϸ��̼��� ����ϴ� ���� ����. */
//	vector<class CChannel*>	m_Channels;
//	
//	_bool					 m_bIsChanged = { false };
//	_bool					 m_bIsTransition = { false };
//	vector<class CChannel*>* m_pPrevChannels = { nullptr };
//	vector<_uint>*			 m_pPrevCurrentKeyFrames = { nullptr };
//	CAnimation*				 m_pPrevAnimation = {nullptr};
//	_float				     m_fTransitionDuration = { 0.2f };
//
//public:
//	void	Transition(_cref_time fTimeDelta, vector<class CBone*> Bones);
//	void	Reset_Animation();
//	void	Set_AnimSpeed(_float fSpeed) { m_fAnimSpeed = fSpeed; }
//	void	Set_AnimAdjustSpeed(_float fSpeed) { m_fAnimAdjustSpeed = fSpeed; }
//	vector<class CChannel*>* Get_Channels() { return &m_Channels; }
//	vector<_uint>* Get_CurrentKeyFrames() { return &m_CurrentKeyFrames; }
//
//	_bool	Is_Finished() { return m_isFinished; }
//	_bool	Is_Changed() { return m_bIsChanged; }
//	_bool	Is_Arrived(_uint iTrackFrameIndex) { return iTrackFrameIndex <= m_iTrackFrameIndex; }
//	_bool   Is_Transition() { return m_bIsTransition; }
//	_uint	Get_TrackFrameIndex() { return m_iTrackFrameIndex; }
//
//	const _char*		Get_Name() { return m_szName; }
//	GETSET_EX2(_float, m_fTransitionDuration, TransitionDuration, GET, SET)
//
//public:
//#ifdef _DEBUG
//	static CAnimation* Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones);
//#endif
//	static CAnimation* Create(ANIMATION_DATA AnimationData);
//	CAnimation* Clone();
//	virtual void Free() override;
//};
//
//END