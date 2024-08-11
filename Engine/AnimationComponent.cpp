#include "AnimationComponent.h"

#include "GameInstance.h"
#include "Model_Manager.h"

#include "ComputeShaderComp.h"

//#include "AnimMaskComp.h"
IMPLEMENT_CLONE(CAnimationComponent, CComponent)
IMPLEMENT_CREATE(CAnimationComponent)

CAnimationComponent::CAnimationComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: Base(pDevice, pContext)
	, m_vRootRotateDelta(XMQuaternionIdentity()), m_vRootScaleDelta(1.f, 1.f, 1.f)
{
}

CAnimationComponent::CAnimationComponent(const CAnimationComponent& rhs)
	: Base(rhs)
	, m_pBoneGroup(rhs.m_pBoneGroup)
	, m_pAnimGroup(rhs.m_pAnimGroup)
	, m_MainAnim(rhs.m_MainAnim)
	, m_vRootTransDelta(rhs.m_vRootTransDelta), m_vRootRotateDelta(rhs.m_vRootRotateDelta), m_vRootScaleDelta(rhs.m_vRootScaleDelta)
//	, m_pBoneBlender(rhs.m_pBoneBlender), m_BoneBlenderBuffers(rhs.m_BoneBlenderBuffers)
//	, m_pTransformCreator(rhs.m_pTransformCreator), m_TransformCreateBuffers(rhs.m_TransformCreateBuffers)
	, m_pNotifyComp(rhs.m_pNotifyComp)
{
	Safe_AddRef(m_pBoneGroup);
	Safe_AddRef(m_pAnimGroup);

	m_pNotifyComp = DynPtrCast<CAnimNotifyComp>(m_pNotifyComp->Clone());
}

HRESULT CAnimationComponent::Initialize_Prototype()
{
	/*if (FAILED(Create_BoneBlenderShader()))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CAnimationComponent::Initialize(void* Arg)
{
	return S_OK;
}


void CAnimationComponent::Free()
{
	__super::Free();

	Safe_Release(m_pBoneGroup);
	Safe_Release(m_pAnimGroup);
}


HRESULT CAnimationComponent::Bind_BoneGroup(CBoneGroup* pBoneGroup)
{
	if (pBoneGroup == nullptr)
		RETURN_EFAIL;

	if (nullptr != m_pBoneGroup)
		Safe_Release(m_pBoneGroup);

	m_pBoneGroup = pBoneGroup;
	Safe_AddRef(m_pBoneGroup);

	return S_OK;
}

HRESULT CAnimationComponent::Load_Animations(const wstring& strModelFilePath)
{
	if (nullptr == m_pGameInstance)
		RETURN_EFAIL;

	auto pAnimGroup = m_pGameInstance->Find_AnimGroup(strModelFilePath);
	if (nullptr == pAnimGroup)
		RETURN_EFAIL;

	m_pAnimGroup = pAnimGroup;
	Safe_AddRef(m_pAnimGroup);

	return S_OK;
}

const FBoneAnimData* CAnimationComponent::Find_BoneAnim(_uint iAnimID) const
{
	if (m_pAnimGroup == nullptr)
		return nullptr;

	return m_pAnimGroup->Find_BoneAnim(iAnimID);
}

/*
HRESULT CAnimationComponent::Create_BoneBlenderShader()
{
	m_pBoneBlender = CComputeShaderComp::Create(m_pDevice, m_pContext);
	if (m_pBoneBlender)
	{
		if (FAILED(m_pBoneBlender->Link_Shader(TEXT("CS_AnimationBlend.hlsl"), "main", nullptr)))
			return E_FAIL;

		// �⺻ ���� ����
		D3D11_BUFFER_DESC Desc = {};
		Desc.ByteWidth = sizeof(FKeyFrameTR) * 1024;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		Desc.CPUAccessFlags = 0;
		Desc.StructureByteStride = sizeof(FKeyFrameTR);

		// Input, SRV, UAV�� ����
		if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, 
			m_BoneBlenderBuffers.pInputBuffers[0].GetAddressOf())))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr,
			m_BoneBlenderBuffers.pInputBuffers[1].GetAddressOf())))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr,
			m_BoneBlenderBuffers.pOutputBuffer.GetAddressOf())))
			return E_FAIL;


		Desc.Usage = D3D11_USAGE_STAGING;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		Desc.BindFlags = 0;
		Desc.MiscFlags = 0;

		// Output, ��� ����� ����
		if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr,
			m_BoneBlenderBuffers.pBackBuffer.GetAddressOf())))
			return E_FAIL;

		// ��� ����
		Desc = {};
		Desc.ByteWidth = sizeof(TBoneBlenderConstants);
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr,
			m_BoneBlenderBuffers.pConstantBuffer.GetAddressOf())))
			return E_FAIL;

		// ����� ����




		// SRV ����
		D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV = {};
		DescSRV.Format = DXGI_FORMAT_UNKNOWN;
		DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescSRV.Buffer.ElementWidth = 1024;

		// SRV ����
		if (FAILED(m_pDevice->CreateShaderResourceView(m_BoneBlenderBuffers.pInputBuffers[0].Get(), &DescSRV,
			m_BoneBlenderBuffers.pInputSRVs[0].GetAddressOf())))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_BoneBlenderBuffers.pInputBuffers[1].Get(), &DescSRV,
			m_BoneBlenderBuffers.pInputSRVs[1].GetAddressOf())))
			return E_FAIL;


		// UAV ����
		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.NumElements = 1024;

		if (FAILED(m_pDevice->CreateUnorderedAccessView(m_BoneBlenderBuffers.pOutputBuffer.Get(), &DescUAV,
			m_BoneBlenderBuffers.pOutputUAV.GetAddressOf())))
			return E_FAIL;
	}

	if (FAILED(Create_CreateTransformShader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimationComponent::Compute_BoneBlenderShader(vector<FKeyFrameTR>& TargetTransforms, vector<FKeyFrameTR>& BlendTransforms, _float fBlend)
{
	if (nullptr == m_pBoneBlender.get() || E_FAIL == m_pBoneBlender->IsShader_Ready())
		return E_FAIL;

	// ��� ���� ���ε�
	m_pContext->CSSetConstantBuffers(0, 1, m_BoneBlenderBuffers.pConstantBuffer.GetAddressOf());
	// ���̴� ����
	m_pContext->CSSetShader(m_pBoneBlender->CRef_Shader().Get(), nullptr, 0);

	// SRV ����
	ID3D11ShaderResourceView* pViewNULL = { nullptr };
	m_pContext->CSGetShaderResources(0, 1, &pViewNULL);

	// UAV ����
	m_pContext->CSSetUnorderedAccessViews(0, 1, m_BoneBlenderBuffers.pOutputUAV.GetAddressOf(), nullptr);

	// SRV ����
	ID3D11ShaderResourceView* pSRVs[] = {
		m_BoneBlenderBuffers.pInputSRVs[0].Get(),
		m_BoneBlenderBuffers.pInputSRVs[1].Get()
	};

	m_pContext->CSSetShaderResources(0, 2, pSRVs);

	TBoneBlenderConstants vConstant = { (_uint)TargetTransforms.size(), fBlend };

	// ��� ���ۿ� ����
	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	if (SUCCEEDED(m_pContext->Map(m_BoneBlenderBuffers.pConstantBuffer.Get(), 0,
		D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
	{
		memcpy(MappedResource.pData, &vConstant, sizeof(vConstant));
		m_pContext->Unmap(m_BoneBlenderBuffers.pConstantBuffer.Get(), 0);
	}

	

	// BUFFER1�� ����
	D3D11_BOX Box;
	Box.left = 0; Box.right = sizeof(FKeyFrameTR) * 1024;
	Box.top = 0; Box.bottom = 1;
	Box.front = 0; Box.back = 1;

	m_pContext->UpdateSubresource(m_BoneBlenderBuffers.pInputBuffers[0].Get(),
		0, &Box, TargetTransforms.data(), sizeof(FKeyFrameTR) * 1024, 1);
	

	// BUFFER2�� ����
	m_pContext->UpdateSubresource(m_BoneBlenderBuffers.pInputBuffers[1].Get(),
		0, &Box, BlendTransforms.data(), sizeof(FKeyFrameTR) * 1024, 1);

	// ��� ������
	m_pContext->Dispatch(64, 1, 1);


	// ������¡ ������ ������尡 ������ Ŀ�� ���� ����.
	//// ��� ��� ����
	//m_pContext->CopyResource(m_BoneBlenderBuffers.pBackBuffer.Get(), m_BoneBlenderBuffers.pOutputBuffer.Get());
	//MappedResource = { 0 };
	//if (SUCCEEDED(m_pContext->Map(m_BoneBlenderBuffers.pBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource)))
	//{
	//	memcpy(&TargetTransforms[0], MappedResource.pData, sizeof(FKeyFrameTR) * TargetTransforms.size());
	//	m_pContext->Unmap(m_BoneBlenderBuffers.pBackBuffer.Get(), 0);
	//}

 	return S_OK;
}

HRESULT CAnimationComponent::Create_CreateTransformShader()
{
	m_pTransformCreator = CComputeShaderComp::Create(m_pDevice, m_pContext);
	if (m_pTransformCreator)
	{
		if (FAILED(m_pTransformCreator->Link_Shader(TEXT("CS_KeyFrameToTransform.hlsl"), "main", nullptr)))
			return E_FAIL;

		auto& SrcBuffers = m_BoneBlenderBuffers;
		auto& Buffers = m_TransformCreateBuffers;

		// ��� ���� ����
		Buffers.pConstantBuffer = SrcBuffers.pConstantBuffer;

		// �⺻ ���� ����
		D3D11_BUFFER_DESC Desc = {};
		Desc.ByteWidth = sizeof(FKeyFrameTR) * 1024;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		Desc.CPUAccessFlags = 0;
		Desc.StructureByteStride = sizeof(FKeyFrameTR);

		// Ű������ ���� ����
		if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, Buffers.pKeyframeBuffer.GetAddressOf())))
			return E_FAIL;


		Desc.ByteWidth = sizeof(_float4x4) * 1024;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		Desc.CPUAccessFlags = 0;
		Desc.StructureByteStride = sizeof(_float4x4);


		// Ʈ������ ���� ����
		if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, Buffers.pTransformBuffer.GetAddressOf())))
			return E_FAIL;


		// CPU����� ������¡ ����
		Desc.Usage = D3D11_USAGE_STAGING;
		Desc.BindFlags = 0;
		Desc.MiscFlags = 0;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, Buffers.pStagingBuffer.GetAddressOf())))
			return E_FAIL;


		// SRV ����
		D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV = {};
		DescSRV.Format = DXGI_FORMAT_UNKNOWN;
		DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescSRV.Buffer.ElementWidth = 1024;

		if (FAILED(m_pDevice->CreateShaderResourceView(Buffers.pKeyframeBuffer.Get(), &DescSRV,
			Buffers.pKeyframeSRV.GetAddressOf())))
			return E_FAIL;


		// UAV ����
		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.NumElements = 1024;

		if (FAILED(m_pDevice->CreateUnorderedAccessView(Buffers.pTransformBuffer.Get(), &DescUAV,
			Buffers.pTransformUAV.GetAddressOf())))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimationComponent::Compute_CreateTransformShader(vector<_float4x4>& Transforms, vector<FKeyFrameTR>& Keyframes)
{
	if (nullptr == m_pTransformCreator.get() || E_FAIL == m_pTransformCreator->IsShader_Ready())
		return E_FAIL;

	auto& Buffers = m_TransformCreateBuffers;
	size_t iBufferSize = sizeof(_float4x4) * 1024;

	// ���̴� �ʱ�ȭ
	{
		// ���̴� ����
		m_pContext->CSSetShader(m_pTransformCreator->CRef_Shader().Get(), nullptr, 0);

		// SRV ����
		ID3D11ShaderResourceView* pViewNULL = { nullptr };
		m_pContext->CSSetShaderResources(0, 1, &pViewNULL);

		// UAV ����
		ID3D11UnorderedAccessView* pUAVNull = { nullptr };
		m_pContext->CSSetUnorderedAccessViews(0, 1, &pUAVNull, nullptr);
	}



	// ���ҽ� ���
	{
		// UAV ����
		ID3D11UnorderedAccessView* pUAVs[] = {
			Buffers.pTransformUAV.Get(),
		};
		m_pContext->CSSetUnorderedAccessViews(0, 1, pUAVs, nullptr);

		// SRV ����
		ID3D11ShaderResourceView* pSRVs[] = {
			Buffers.pKeyframeSRV.Get(),
		};
		m_pContext->CSSetShaderResources(0, 1, pSRVs);
	}


	
	// ������ ����ȭ
	{
		TBoneBlenderConstants vConstant = { (_uint)Transforms.size() };

		// ��� ���ۿ� ����
		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		if (SUCCEEDED(m_pContext->Map(Buffers.pConstantBuffer.Get(), 0,
			D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
		{
			memcpy(MappedResource.pData, &vConstant, sizeof(vConstant));
			m_pContext->Unmap(Buffers.pConstantBuffer.Get(), 0);
		}

		// Ű������ ����
		D3D11_BOX Box;
		Box.left = 0; Box.right = sizeof(FKeyFrameTR) * 1024;
		Box.top = 0; Box.bottom = 1;
		Box.front = 0; Box.back = 1;

		m_pContext->UpdateSubresource(Buffers.pKeyframeBuffer.Get(),
			0, &Box, Keyframes.data(), sizeof(FKeyFrameTR) * Keyframes.size(), 1);

		// ������ ����� Ű������ ������� ����, ���� �߸��Ǹ� ���� ����
		//m_pContext->CopyResource(Buffers.pKeyframeBuffer.Get(), m_BoneBlenderBuffers.pOutputBuffer.Get());
	}
	

	
	// ���
	{
		// ��� ��� ������
		m_pContext->Dispatch(1024, 1, 1);
	}
	
	
	
	// ������¡ ������ ������尡 ������ Ŀ�� ������� ����
	//// ��� ó��
	//{
	//	// ������¡ ���ۿ� Ʈ������ ���� �����ϱ�
	//	m_pContext->CopyResource(Buffers.pStagingBuffer.Get(), Buffers.pTransformBuffer.Get());
	//	
	//	// �ܺ� Ʈ�������� �������ֱ�
	//	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	//	if (SUCCEEDED(m_pContext->Map(Buffers.pStagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource)))
	//	{
	//		
	//		memcpy(&Transforms[0], MappedResource.pData, sizeof(_float4x4) * Transforms.size());
	//		m_pContext->Unmap(Buffers.pStagingBuffer.Get(), 0);
	//	}
	//}
	

	return S_OK;
}
*/

_bool CAnimationComponent::IsAnimation_Finished()
{
	return (m_MainAnim.Cur.bIsReverse) ? (m_MainAnim.Cur.fTrackPos <= 0) : (m_MainAnim.Cur.fTrackPos >= m_MainAnim.Cur.fDuration);
}

_bool CAnimationComponent::IsAnimation_Finished(wstring strAnimName)
{
	auto pBoneAnimData = m_pAnimGroup->Find_BoneAnim(strAnimName);
	if (pBoneAnimData == nullptr)
		return false;

	if (m_MainAnim.Cur.iAnimID != pBoneAnimData->iID)
		return false;	

		return (m_MainAnim.Cur.bIsReverse) ? (m_MainAnim.Cur.fTrackPos <= 0) : (m_MainAnim.Cur.fTrackPos >= m_MainAnim.Cur.fDuration);
}

_bool CAnimationComponent::IsCurrentAnimation(wstring& strAnimName)
{
	if(m_MainAnim.Cur.strAnimName == WstrToStr(strAnimName))
		return true;

	return false;
}

_bool CAnimationComponent::IsAnimation_UpTo(_float m_fTrackPos)
{
	return (m_MainAnim.Cur.bIsReverse) ? (m_MainAnim.Cur.fTrackPos <= m_fTrackPos) : (m_MainAnim.Cur.fTrackPos >= m_fTrackPos);
}

_bool CAnimationComponent::IsAnimation_Frame_Once(_float m_fTrackPos)
{
	_bool bResult = signbit(m_MainAnim.Cur.fTrackPos - m_fTrackPos) != signbit(m_MainAnim.Cur.fPrevTrackPos - m_fTrackPos)
		|| (signbit(m_MainAnim.Cur.fTrackPos - m_MainAnim.Cur.fPrevTrackPos) == 0 
			&& !m_MainAnim.Cur.bIsReverse && (m_fTrackPos == 0.f))
		|| (signbit(m_MainAnim.Cur.fTrackPos - m_MainAnim.Cur.fPrevTrackPos) == 0 
			&& m_MainAnim.Cur.bIsReverse && (m_fTrackPos == m_MainAnim.Cur.fDuration));
	return bResult;
}

_bool CAnimationComponent::IsAnimation_Range(_float fTrackMin, _float fTrackMax)
{
	return ((fTrackMin <= m_MainAnim.Cur.fTrackPos) && (m_MainAnim.Cur.fTrackPos <= fTrackMax))
		|| ((m_MainAnim.Cur.fPrevTrackPos <= fTrackMax) && (fTrackMax <= m_MainAnim.Cur.fTrackPos))
		|| ((m_MainAnim.Cur.fTrackPos <= fTrackMax) && (fTrackMax <= m_MainAnim.Cur.fPrevTrackPos));
}

_bool CAnimationComponent::IsCurrentAnimation(wstring strAnimName)

{
	auto pBoneAnimData = m_pAnimGroup->Find_BoneAnim(strAnimName);
	if (pBoneAnimData == nullptr)
		return false;

	return m_MainAnim.Cur.iAnimID == pBoneAnimData->iID;
}

void CAnimationComponent::Set_Animation(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop, 
	_bool bIsTransition, _float fTransitionSpeed, _bool bReverse,
	_bool bIsRootReset, ERootAnimType eRootAnimType) 
{
	if (iAnimIndex < 0 || iAnimIndex >= m_pAnimGroup->Get_NumAnims())
		return;

	auto pBoneAnimData = m_pAnimGroup->Find_BoneAnim(iAnimIndex);
	if (pBoneAnimData == nullptr)
		return;

	// ���� �ִϸ��̼� ����
	if (m_MainAnim.Cur.iAnimID != iAnimIndex || !m_MainAnim.Prev.bIsLoop )
	{
		if (m_MainAnim.Cur.iAnimID != iAnimIndex)
		{
			m_MainAnim.Prev = m_MainAnim.Cur;
			bIsTransition ? m_MainAnim.fTransBlend.Reset() : m_MainAnim.fTransBlend.Set_Max();
			m_MainAnim.fTransSpeed = fTransitionSpeed;
		}

		m_MainAnim.Cur.iAnimID = iAnimIndex;
		m_MainAnim.Cur.strAnimName = WstrToStr(pBoneAnimData->strName);
		m_MainAnim.Cur.fDuration = pBoneAnimData->fDuration;
		m_MainAnim.Cur.fTickPerSeconds = pBoneAnimData->fTickPerSecond;
		m_MainAnim.Cur.fSpeedMultiply = fSpeedMultiply;
		m_MainAnim.Cur.bIsLoop = bIsLoop;
		m_MainAnim.Cur.bIsReverse = bReverse;
		m_MainAnim.Cur.fTrackPos = (!bReverse) ? 0.f : m_MainAnim.Cur.fDuration;
		m_MainAnim.Cur.fPrevTrackPos = m_MainAnim.Cur.fTrackPos;
		m_MainAnim.Cur.bIsTransition = bIsTransition;
		m_MainAnim.Cur.eRootMotion_TransApplyType = eRootAnimType;
		ResetTrigger_AnimNotifyByCurAnim();
		Set_AnimNameToNotify();

		if (bIsRootReset)
		{
			RootReset();
		}

     	_matrix RootMatrix = m_pBoneGroup->CRef_BoneCombinedTransforms()[0];
		auto pTransChannel = pBoneAnimData->Find_AnimChannelData(m_pBoneGroup->CRef_RootTransBoneInd());
		if (pTransChannel)
		{
			FKeyFrameTR TransKeyFrame = pTransChannel->Interpolated_KeyFrame(m_MainAnim.Cur.fTrackPos);
			m_vRootTrans = m_vRootTrans_Prev = XMVector3TransformCoord(TransKeyFrame.vPos, RootMatrix);
		}
	}

	cout << WstrToStr(pBoneAnimData->strName) << endl;
}

void CAnimationComponent::Set_Animation(const wstring& strAnimName, _float fSpeedMultiply, _bool bIsLoop, 
	_bool bIsTransition, _float fTransitionSpeed , _bool bReverse, _bool bIsRootReset, ERootAnimType eRootAnimType)
{
	auto pBoneAnimData = m_pAnimGroup->Find_BoneAnim(strAnimName);
	if (pBoneAnimData == nullptr)
		return;

	// ���� �ִϸ��̼� ����
	if (m_MainAnim.Cur.iAnimID != pBoneAnimData->iID || !m_MainAnim.Prev.bIsLoop)
	{
		if (m_MainAnim.Cur.iAnimID != pBoneAnimData->iID)
		{
			m_MainAnim.Prev = m_MainAnim.Cur;
			bIsTransition ? m_MainAnim.fTransBlend.Reset() : m_MainAnim.fTransBlend.Set_Max();
			m_MainAnim.fTransSpeed = fTransitionSpeed;
		}		

		m_MainAnim.Cur.iAnimID = pBoneAnimData->iID;
		m_MainAnim.Cur.strAnimName = WstrToStr(pBoneAnimData->strName);
		m_MainAnim.Cur.fDuration = pBoneAnimData->fDuration;
		m_MainAnim.Cur.fTickPerSeconds = pBoneAnimData->fTickPerSecond;
		m_MainAnim.Cur.fSpeedMultiply = fSpeedMultiply;
		m_MainAnim.Cur.bIsLoop = bIsLoop;
		m_MainAnim.Cur.bIsReverse = bReverse;
		m_MainAnim.Cur.fTrackPos = (!bReverse) ? 0.f : m_MainAnim.Cur.fDuration;
		m_MainAnim.Cur.fPrevTrackPos = m_MainAnim.Cur.fTrackPos;
		m_MainAnim.Cur.bIsTransition = bIsTransition;
		m_MainAnim.Cur.eRootMotion_TransApplyType = eRootAnimType;
		ResetTrigger_AnimNotifyByCurAnim();
		Set_AnimNameToNotify();

		if (bIsRootReset)
		{
			RootReset();
			//cout << "��Ʈ �ʱ�ȭ" << endl;
		}

		//
		_matrix RootMatrix = m_pBoneGroup->CRef_BoneCombinedTransforms()[0];
		auto pTransChannel = pBoneAnimData->Find_AnimChannelData(m_pBoneGroup->CRef_RootTransBoneInd());
		if (pTransChannel)
		{
			FKeyFrameTR TransKeyFrame = pTransChannel->Interpolated_KeyFrame(m_MainAnim.Cur.fTrackPos);
			m_vRootTrans = m_vRootTrans_Prev = XMVector3TransformCoord(TransKeyFrame.vPos, RootMatrix);
		}
	}

	//cout << WstrToStr(pBoneAnimData->strName) << endl;
}

void CAnimationComponent::Set_AnimationMaintain(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop, _bool bReverse, _float fTransitionSpeed, _bool bIsRootReset)
{
	if (iAnimIndex < 0 || iAnimIndex >= m_pAnimGroup->Get_NumAnims())
		return;

	auto pBoneAnimData = m_pAnimGroup->Find_BoneAnim(iAnimIndex);
	if (pBoneAnimData == nullptr)
		return;

	// ���� �ִϸ��̼� ����
	if (m_MainAnim.Cur.iAnimID != iAnimIndex)
	{
		m_MainAnim.Prev = m_MainAnim.Cur;
		m_MainAnim.fTransBlend.Reset();
		m_MainAnim.fTransSpeed = fTransitionSpeed;
	}

	m_MainAnim.Cur.iAnimID = iAnimIndex;
	m_MainAnim.Cur.fDuration = pBoneAnimData->fDuration;
	m_MainAnim.Cur.fTickPerSeconds = pBoneAnimData->fTickPerSecond;
	m_MainAnim.Cur.fSpeedMultiply = fSpeedMultiply;
	m_MainAnim.Cur.bIsLoop = bIsLoop;
	m_MainAnim.Cur.bIsReverse = bReverse;
	ResetTrigger_AnimNotifyByCurAnim();
	Set_AnimNameToNotify();

	if (bIsRootReset)
	{
		RootReset();
	}
}

void CAnimationComponent::Tick_AnimTime(_cref_time fTimeDelta)
{
	_bool IsLooped = false;
	_float fTiggerTrackPos = m_MainAnim.Cur.fTrackPos;

	m_MainAnim.Cur.fPrevTrackPos = m_MainAnim.Cur.fTrackPos;
	m_MainAnim.Cur.fTrackPos += m_MainAnim.Cur.fTickPerSeconds * m_MainAnim.Cur.fSpeedMultiply * fTimeDelta * (m_MainAnim.Cur.bIsReverse ? -1 : 1);
	if (!m_MainAnim.Cur.bIsReverse)
	{
		if (m_MainAnim.Cur.fTrackPos > m_MainAnim.Cur.fDuration)
		{
			auto pBoneAnimData = m_pAnimGroup->Find_BoneAnim(m_MainAnim.Cur.iAnimID);
			if (pBoneAnimData == nullptr)
				return;

			fTiggerTrackPos = m_MainAnim.Cur.fDuration;
			// ������
			if (m_MainAnim.Cur.bIsLoop)
			{
				m_MainAnim.Cur.fTrackPos = 0.f;
				RootReset();
				ResetTrigger_AnimNotifyByCurAnim();
				Set_AnimNameToNotify();

				_matrix RootMatrix = m_pBoneGroup->CRef_BoneCombinedTransforms()[0];
				auto pTransChannel = pBoneAnimData->Find_AnimChannelData(m_pBoneGroup->CRef_RootTransBoneInd());
				if (pTransChannel)
				{
					FKeyFrameTR TransKeyFrame = pTransChannel->Interpolated_KeyFrame(m_MainAnim.Cur.fTrackPos);
					m_vRootTrans = m_vRootTrans_Prev = XMVector3TransformCoord(TransKeyFrame.vPos, RootMatrix);
				}
			}
			// ���� �ƴ� ��
			else
				m_MainAnim.Cur.fTrackPos = m_MainAnim.Cur.fDuration;
		}
	}
	else
	{
		if (m_MainAnim.Cur.fTrackPos < 0.f)
		{
			auto pBoneAnimData = m_pAnimGroup->Find_BoneAnim(m_MainAnim.Cur.iAnimID);
			if (pBoneAnimData == nullptr)
				return;

			fTiggerTrackPos = m_MainAnim.Cur.fDuration;
			if (m_MainAnim.Cur.bIsLoop)
			{
				m_MainAnim.Cur.fTrackPos = m_MainAnim.Cur.fDuration;
				RootReset();
				ResetTrigger_AnimNotifyByCurAnim();

				_matrix RootMatrix = m_pBoneGroup->CRef_BoneCombinedTransforms()[0];
				auto pTransChannel = pBoneAnimData->Find_AnimChannelData(m_pBoneGroup->CRef_RootTransBoneInd());
				if (pTransChannel)
				{
					FKeyFrameTR TransKeyFrame = pTransChannel->Interpolated_KeyFrame(m_MainAnim.Cur.fTrackPos);
					m_vRootTrans = m_vRootTrans_Prev = XMVector3TransformCoord(TransKeyFrame.vPos, RootMatrix);
				}
			}
			else
				m_MainAnim.Cur.fTrackPos = 0.f;
		}
	}
	

	if (!m_MainAnim.fTransBlend.IsMax() && m_MainAnim.Cur.bIsTransition)
	{
		m_MainAnim.Prev.fPrevTrackPos = m_MainAnim.Prev.fTrackPos;
		m_MainAnim.Prev.fTrackPos += m_MainAnim.Prev.fTickPerSeconds * m_MainAnim.Prev.fSpeedMultiply * fTimeDelta * (m_MainAnim.Prev.bIsReverse ? -1 : 1);
		if (!m_MainAnim.Prev.bIsReverse)
		{
			if (m_MainAnim.Prev.fTrackPos > m_MainAnim.Prev.fDuration)
				m_MainAnim.Prev.fTrackPos = (m_MainAnim.Prev.bIsLoop) ? (0.f) : (m_MainAnim.Prev.fDuration);
		}
		else
		{
			if (m_MainAnim.Prev.fTrackPos < 0.f)
				m_MainAnim.Prev.fTrackPos = (m_MainAnim.Prev.bIsLoop) ? (m_MainAnim.Prev.fDuration) : (0.f);
		}
	}

	// Ʈ���� �۵�
	Trigger_AnimNotifyByCurAnim(fTiggerTrackPos);
}

void CAnimationComponent::Set_AnimTrackPos(_int iIndex)
{
	m_MainAnim.Cur.fTrackPos = Cast<_float>(iIndex);
	m_MainAnim.Cur.fPrevTrackPos = m_MainAnim.Cur.fTrackPos;
}

string CAnimationComponent::Get_PrevAnimName()
{
	return m_MainAnim.Prev.strAnimName;
}

string CAnimationComponent::Get_CurrentAnimName()
{
	return m_MainAnim.Cur.strAnimName;
}

_float CAnimationComponent::Get_AnimTrackPos()
{
	return m_MainAnim.Cur.fTrackPos;
}

_float CAnimationComponent::Get_AnimDuration()
{
	return m_MainAnim.Cur.fDuration;
}

void CAnimationComponent::Invalidate_Animation(_cref_time fTimeDelta)
{
	if (!m_pBoneGroup || !m_pAnimGroup)
		return;

#pragma region ������
	/*vector<pair<_matrix, _bool>> vecBoneMatrices;
	vector<FKeyFrameTR> vecKeyFrames;
	vector<FKeyFrameInterpolate> vecKeyFrameInters;*/

	//vecBoneMatrices.resize(iNumBones, {});
	//vecKeyFrames.resize(iNumBones, {});
	//if (m_fTransitionGauge < 0.2f && m_MainAnim.Cur.bIsTransition)
	//{
	//	FAnimInterpolate AnimInter = {};
	//	AnimInter.iAnimID = m_MainAnim.Cur.iAnimID;
	//	AnimInter.fTrackPos = m_MainAnim.Cur.fTrackPos;
	//	AnimInter.fWeight = m_fTransitionGauge * 5.f;
	//	AnimInter.vecChannelIDs.reserve(iNumBones);
	//	for (_uint i = 0; i < iNumBones; i++)
	//		AnimInter.vecChannelIDs.push_back(i);

	//	FAnimInterpolate PrevAnimInter = AnimInter;
	//	PrevAnimInter.iAnimID = m_MainAnim.Prev.iAnimID;
	//	PrevAnimInter.fTrackPos = m_MainAnim.Prev.fTrackPos;
	//	PrevAnimInter.fWeight = 1.f - m_fTransitionGauge * 5.f;
	//	FAnimInterpolate ArrAnimInter[2] = {
	//	   AnimInter, PrevAnimInter
	//	};

	//	// ����ũ �ִϸ��̼ǳ��� �����Ѵ�.
	//	m_pAnimGroup->Interpolated_Anims(vecKeyFrames.data(), vecKeyFrames.size(), ArrAnimInter, 2);
	//}
	//// ������ KeyFrameInterpolate�� �� �ִϸ��̼����κ��� ���´�.
	//else
	//{
	//	FAnimInterpolate AnimInter = {};
	//	AnimInter.iAnimID = m_MainAnim.Cur.iAnimID;
	//	AnimInter.fTrackPos = m_MainAnim.Cur.fTrackPos;
	//	AnimInter.fWeight = 1.f;
	//	AnimInter.vecChannelIDs.reserve(iNumBones);
	//	for (_uint i = 0; i < iNumBones; i++)
	//		AnimInter.vecChannelIDs.push_back(i);

	//	m_pAnimGroup->Interpolated_Anims(vecKeyFrames.data(), vecKeyFrames.size(), &AnimInter, 1);
	//}

	//		// ��Ʈ �̵� ��
	//_uint iRootBoneIndex = m_pBoneGroup->CRef_RootTransBoneInd();
	//_float4x4 RootBoneTransform = XMMatrixIdentity();

	//// ���� �ִϸ��̼��� �� �ִϸ��̼�
	//const FBoneAnimData* pBoneAnimData = m_pAnimGroup->Find_BoneAnim(m_MainAnim.Cur.iAnimID);
	//for (_uint i = 0; i < iNumBones; i++)
	//{
	//	const FBoneAnimChannelData* pBoneAnimChannelData = pBoneAnimData->Find_AnimChannelData(i);

	//	if (nullptr != pBoneAnimChannelData)
	//	{
	//		// ��Ʈ�� ��� �ʱ�ȭ
	//		if (i == iRootBoneIndex)
	//		{
	//			vecBoneMatrices[i] = { IdenetityMatrix, true };
	//			RootBoneTransform = {
	//			   XMMatrixAffineTransformation(
	//				  XMLoadFloat3(&vecKeyFrames[i].vScale),
	//				  XMQuaternionIdentity(),
	//				  XMLoadFloat4(&vecKeyFrames[i].qtRot),
	//				  XMLoadFloat3(&vecKeyFrames[i].vPos))
	//			};
	//		}
	//		else
	//			vecBoneMatrices[i] =
	//		{ XMMatrixAffineTransformation(
	//		   XMLoadFloat3(&vecKeyFrames[i].vScale),
	//		   XMQuaternionIdentity(),
	//		   XMLoadFloat4(&vecKeyFrames[i].qtRot),
	//		   XMLoadFloat3(&vecKeyFrames[i].vPos)), true };
	//	}
	//	else
	//	{
	//		vecBoneMatrices[i] = { IdenetityMatrix, false };
	//	}
	//}
#pragma endregion
	
	// �� ����
	_uint iNumBones = m_pBoneGroup->CRef_BoneDatas_Count();
	_matrix IdenetityMatrix = XMMatrixIdentity();
	// ��Ʈ �̵� ��
	_uint iRootTransIndex = m_pBoneGroup->CRef_RootTransBoneInd();
	_uint iRootRotateIndex = m_pBoneGroup->CRef_RootRotateBoneInd();
	_uint iRootScaleIndex = m_pBoneGroup->CRef_RootScaleBoneInd();
	

	vector<_float4x4> BoneMatrices;
	vector<_bool> BoneApplies;
	BoneMatrices.resize(iNumBones);
	BoneApplies.resize(iNumBones, true);

	vector<FKeyFrameTR> CurKeyFrames;
	

	// �ִϸ��̼� ����
	CurKeyFrames.swap(m_pAnimGroup->Provide_AnimKeyFrames(m_MainAnim.Cur.iAnimID, m_MainAnim.Cur.fTrackPos, *m_pBoneGroup));
	// Ʈ������ ����
	if (!m_MainAnim.fTransBlend.IsMax() && m_MainAnim.Cur.bIsTransition)
	{
		vector<FKeyFrameTR> PrevKeyFrames;
		PrevKeyFrames.swap(m_pAnimGroup->Provide_AnimKeyFrames(m_MainAnim.Prev.iAnimID, m_MainAnim.Prev.fTrackPos, *m_pBoneGroup));

		_uint iKeyFrameSize = Cast<_uint>(CurKeyFrames.size());
		for (_uint i = 0; i < iKeyFrameSize; i++)
		{
			// ���� �ִ�
			if (CurKeyFrames[i].iBoneID == -1 || PrevKeyFrames[i].iBoneID == -1
				|| i == iRootTransIndex || i == iRootRotateIndex || i == iRootScaleIndex)
				continue;

			CurKeyFrames[i].vPos = XMVectorLerp(PrevKeyFrames[i].vPos, CurKeyFrames[i].vPos, m_MainAnim.fTransBlend.Get_Percent());
			CurKeyFrames[i].qtRot = XMQuaternionSlerp(PrevKeyFrames[i].qtRot, CurKeyFrames[i].qtRot, m_MainAnim.fTransBlend.Get_Percent());
			CurKeyFrames[i].vScale = XMVectorLerp(PrevKeyFrames[i].vScale, CurKeyFrames[i].vScale, m_MainAnim.fTransBlend.Get_Percent());
		}

		if (m_MainAnim.Cur.iAnimID != m_MainAnim.Prev.iAnimID)
			m_MainAnim.fTransBlend.Increase(m_MainAnim.fTransSpeed * 5.f * fTimeDelta);
		else
			m_MainAnim.fTransBlend.Set_Max();
	}


	// ����ŷ �ִϸ��̼� ����, �ִϸ��̼� �и��� ����. ���̼�, ��ü�� ���� ���� ���� ��ǥ���̴�.
	for (_uint i = 0; i < (_uint)m_MaskAnim.size(); i++)
	{
		// �̻�� �� ���
		if (!m_MaskAnim[i].bUsing)
			continue;

		vector<FKeyFrameTR> CurMaskKeyFrames;
		CurMaskKeyFrames.swap(m_pAnimGroup->Provide_AnimKeyFrames_ParentedDown(
			m_MaskAnim[i].Cur.iAnimID, m_MaskAnim[i].Cur.fTrackPos, *m_pBoneGroup
			, m_MaskAnim[i].Cur.strStartBoneName, m_MaskAnim[i].Cur.strEndBoneNames
			, m_MaskAnim[i].Cur.iRecursive
		));

		// 
		if (!m_MaskAnim[i].fTransBlend.IsMax() && m_MaskAnim[i].Cur.bIsTransition)
		{
			vector<FKeyFrameTR> PrevMaskKeyFrames;
			PrevMaskKeyFrames.swap(m_pAnimGroup->Provide_AnimKeyFrames_ParentedDown(
				m_MaskAnim[i].Prev.iAnimID, m_MaskAnim[i].Prev.fTrackPos, *m_pBoneGroup
				, m_MaskAnim[i].Prev.strStartBoneName, m_MaskAnim[i].Prev.strEndBoneNames
				, m_MaskAnim[i].Prev.iRecursive
			));

			_uint iKeyFrameSize = Cast<_uint>(CurMaskKeyFrames.size());
			for (_uint j = 0; j < iKeyFrameSize; j++)
			{
				// ���� �ִ�
				if (CurMaskKeyFrames[j].iBoneID == -1 || PrevMaskKeyFrames[j].iBoneID == -1
					|| j == iRootTransIndex || j == iRootRotateIndex || j == iRootScaleIndex)
					continue;

				CurMaskKeyFrames[j].vPos = XMVectorLerp(PrevMaskKeyFrames[j].vPos, CurMaskKeyFrames[j].vPos, m_MaskAnim[i].fTransBlend.Get_Percent());
				CurMaskKeyFrames[j].qtRot = XMQuaternionSlerp(PrevMaskKeyFrames[j].qtRot, CurMaskKeyFrames[j].qtRot, m_MaskAnim[i].fTransBlend.Get_Percent());
				CurMaskKeyFrames[j].vScale = XMVectorLerp(PrevMaskKeyFrames[j].vScale, CurMaskKeyFrames[j].vScale, m_MaskAnim[i].fTransBlend.Get_Percent());
			}

			if (m_MaskAnim[i].Cur.iAnimID != m_MaskAnim[i].Prev.iAnimID)
				m_MaskAnim[i].fTransBlend.Increase(m_MaskAnim[i].fTransSpeed * 5.f * fTimeDelta);
			else
				m_MaskAnim[i].fTransBlend.Set_Max();
		}

		// ���� �ִϸ��̼ǿ� �����ϴ� �ڵ�
		_uint iKeyFrameSize = Cast<_uint>(CurMaskKeyFrames.size());
		for (_uint j = 0; j < iKeyFrameSize; j++)
		{
			// ���� �ִ�
			if (CurMaskKeyFrames[j].iBoneID == -1
				|| j == iRootTransIndex || j == iRootRotateIndex || j == iRootScaleIndex)
				continue;

			if (m_MaskAnim[i].bEndMasking && !m_MaskAnim[i].fFadeOutBlend.IsMax())
			{
				CurKeyFrames[j].vPos = XMVectorLerp(CurMaskKeyFrames[j].vPos, CurKeyFrames[j].vPos, m_MaskAnim[i].fFadeOutBlend.fCur);
				CurKeyFrames[j].qtRot = XMQuaternionSlerp(CurMaskKeyFrames[j].qtRot, CurKeyFrames[j].qtRot, m_MaskAnim[i].fFadeOutBlend.fCur);
				CurKeyFrames[j].vScale = XMVectorLerp(CurMaskKeyFrames[j].vScale, CurKeyFrames[j].vScale, m_MaskAnim[i].fFadeOutBlend.fCur);
			}
			else if (!m_MaskAnim[i].bEndMasking && !m_MaskAnim[i].fFadeOutBlend.IsMax())
			{
				CurKeyFrames[j].vPos = XMVectorLerp(CurMaskKeyFrames[j].vPos, CurKeyFrames[j].vPos, 1.f - m_MaskAnim[i].fFadeOutBlend.fCur);
				CurKeyFrames[j].qtRot = XMQuaternionSlerp(CurMaskKeyFrames[j].qtRot, CurKeyFrames[j].qtRot, 1.f - m_MaskAnim[i].fFadeOutBlend.fCur);
				CurKeyFrames[j].vScale = XMVectorLerp(CurMaskKeyFrames[j].vScale, CurKeyFrames[j].vScale, 1.f - m_MaskAnim[i].fFadeOutBlend.fCur);
			}
			else
			{
				CurKeyFrames[j].vPos = CurMaskKeyFrames[j].vPos;
				CurKeyFrames[j].qtRot = CurMaskKeyFrames[j].qtRot;
				CurKeyFrames[j].vScale = CurMaskKeyFrames[j].vScale;
			}
		}

		// �ִϸ��̼� ����� ����
		if (m_MaskAnim[i].fFadeOutBlend.Increase(fTimeDelta * 5.f * m_MaskAnim[i].fFadeOutSpeed))
		{
			if (m_MaskAnim[i].bEndMasking)
				m_MaskAnim[i].bUsing = false;
		}
	}
	

	// ADD �ִϸ��̼� ����, ������ ���⿡ ���°� �´°ǰ� ������. �ǰ�ȿ���� ���δ�.
	for (_uint i = 0; i < (_uint)m_AddAnims.size(); i++)
	{
		auto& AddAnim = m_AddAnims[i];

		// �̻������ �� ���
		if (!AddAnim.bUsing)
			continue;

		vector<FKeyFrameTR> AddKeyFrames;
		AddKeyFrames.swap(m_pAnimGroup->Provide_AnimKeyFrames_ParentedDown(
			AddAnim.Add.iAnimID, AddAnim.Add.fTrackPos, *m_pBoneGroup
			, AddAnim.Add.strStartBoneName, AddAnim.Add.strEndBoneNames, AddAnim.Add.iRecursive));

		//auto& OffsetTransforms = m_pBoneGroup->CRef_BoneOffsetTransforms();

		/*FRapidJson::Open_Json(GI()->MainDataPath() + L"JSON_Script/Test.json")
			.Read_Data("Test", m_fAddAnim_Weight);*/

		_uint iKeyFrameSize = Cast<_uint>(CurKeyFrames.size());
		for (_uint j = 0; j < iKeyFrameSize; j++)
		{
			// ���� �ִ�
			if (CurKeyFrames[j].iBoneID == -1 || AddKeyFrames[j].iBoneID == -1
				|| j == iRootTransIndex || j == iRootRotateIndex || j == iRootScaleIndex)
				continue;

			/*_matrix TempMatrix = XMMatrixAffineTransformation(AddKeyFrames[i].vScale,
				XMQuaternionIdentity(), AddKeyFrames[i].qtRot, AddKeyFrames[i].vPos);*/

				//_float3 vEuler = Get_RotEulerFromMatrix(TempMatrix);

				/*cout << i << ": " << vEuler.x
					<< " " << vEuler.y
					<< " " << vEuler.z << endl;*/
					//CurKeyFrames[i].vPos += AddKeyFrames[i].vPos * fTest;
					/*_float fQuatLength = XMVectorGetX(XMQuaternionLength(XMLoadFloat4(&CurKeyFrames[i].qtRot)));
					_float fPosLength = AddKeyFrames[i].vPos.length();

					cout << i << ": " << fQuatLength << ", " << fPosLength << endl;*/

					/*if (AddKeyFrames[i].vPos.length() > 0.01f)
						continue;*/

			CurKeyFrames[j].qtRot
				= XMQuaternionMultiply(CurKeyFrames[j].qtRot
					, XMQuaternionSlerp(XMQuaternionIdentity()
						, AddKeyFrames[j].qtRot, AddAnim.fWeight));
		}
	}


	// TODO: (���� ������), ������ �ִϸ��̼� ����, �ַ� ���� Ư�� ��ġ�� ���ϰų� ���� Ư�� ��ġ�� ���ϴ� ���� 
	// ��Ÿ�� ������ �ִϸ��̼��� ���⿡ ���Եȴ�.




	_float4x4 RootTransTransform = XMMatrixIdentity();
	_float4x4 RootRotateTransform = XMMatrixIdentity();
	_float4x4 RootScaleTransform = XMMatrixIdentity();
	_float4x4 RootBoneTransform = XMMatrixIdentity();

	// ���� �ִϸ��̼��� �� �ִϸ��̼�
	for (_uint i = 0; i < iNumBones; i++)
	{
		if (CurKeyFrames[i].iBoneID != -1)
		{
			// ��Ʈ�� ��� �ʱ�ȭ
			if (i == iRootTransIndex || i == 1)
				//|| i == iRootRotateIndex 
				//|| i == iRootScaleIndex)
			{
				if (iRootTransIndex != 1)
				{
					BoneMatrices[i] = {
					   XMMatrixAffineTransformation(
						  XMLoadFloat3(&CurKeyFrames[i].vScale),
						  XMQuaternionIdentity(),
						  XMLoadFloat4(&CurKeyFrames[i].qtRot),
						  XMVectorSet(0.f, 0.f, 0.f, 1.f))
					};
				}
				else
					BoneMatrices[i] = IdenetityMatrix;
				BoneApplies[i] = true;
				RootBoneTransform = {
				   XMMatrixAffineTransformation(
					  XMLoadFloat3(&CurKeyFrames[i].vScale),
					  XMQuaternionIdentity(),
					  XMLoadFloat4(&CurKeyFrames[i].qtRot),
					  XMLoadFloat3(&CurKeyFrames[i].vPos))
				};
			}
			else
			{
				BoneMatrices[i] = { 
					XMMatrixAffineTransformation(
					XMLoadFloat3(&CurKeyFrames[i].vScale),
					XMQuaternionIdentity(),
					XMLoadFloat4(&CurKeyFrames[i].qtRot),
					XMLoadFloat3(&CurKeyFrames[i].vPos)) };
				BoneApplies[i] = true;
			}
		}
		else
		{
			BoneMatrices[i] = IdenetityMatrix;
			BoneApplies[i] = false;
		}
	}


	// ��Ʈ�� ���ϱ�, ������ 0.01, ȸ�� Y 180��.
	RootBoneTransform *= RootScaleTransform * RootRotateTransform * RootTransTransform * m_pBoneGroup->CRef_BoneTransforms()[0];
	_vector vSimPos, vSimRot, vSimScale;
	XMMatrixDecompose(&vSimScale, &vSimRot, &vSimPos, RootBoneTransform);
	m_vRootTrans_Prev = m_vRootTrans;
	m_vRootTrans = vSimPos;
	m_vRootTransDelta = Apply_TransDeltaByRootAnimType(m_MainAnim.Cur.eRootMotion_TransApplyType, m_vRootTrans - m_vRootTrans_Prev);

	//// �� Ʈ�������� �ִϸ��̼� ����
	m_pBoneGroup->Invalidate_BoneTransforms(BoneMatrices, BoneApplies);
}

/*
void CAnimationComponent::Invalidate_AnimationCShader(_cref_time fTimeDelta)
{
	if (nullptr == m_pBoneGroup || nullptr == m_pAnimGroup)
		return;

	_uint iNumBones = m_pBoneGroup->CRef_BoneDatas_Count();
	_float4x4 IdenetityFloat4x4 = XMMatrixIdentity();

	if (iNumBones == 0)
	{
		OutputDebugString(L"���� �����ϴ�.");
		return;
	}
	if (m_pAnimGroup->Get_NumAnims() == 0)
	{
		OutputDebugString(L"�ִϸ��̼��� �����ϴ�.");
		return;
	}


	
	// Ű������ ���
	vector<FKeyFrameTR> CurKeyFrames;
	vector<FKeyFrameTR> PrevKeyFrames;
	CurKeyFrames.swap(m_pAnimGroup->Provide_AnimKeyFrames(m_MainAnim.Cur.iAnimID, m_MainAnim.Cur.fTrackPos, *m_pBoneGroup));

	
	// ���� �ʿ�� ��ǻƮ ���̴��� �۵�
	if (m_MainAnim.Cur.bIsTransition && m_MainAnim.Cur.iAnimID != m_MainAnim.Prev.iAnimID && !m_MainAnim.fTransBlend.IsMax())
	{
		PrevKeyFrames.swap(m_pAnimGroup->Provide_AnimKeyFrames(m_MainAnim.Prev.iAnimID, m_MainAnim.Prev.fTrackPos, *m_pBoneGroup));
		
		Compute_BoneBlenderShader(CurKeyFrames, PrevKeyFrames, m_MainAnim.fTransBlend.Get_Percent());

		// Ʈ�����ǰ� �ø�
		m_MainAnim.fTransBlend.Increase(m_MainAnim.fTransSpeed * 5.f * fTimeDelta);
	}

	
	

	// ��� ����
	vector<_float4x4> BoneMatrices;
	vector<_bool>	TransformApplies;
	BoneMatrices.resize(iNumBones, XMMatrixIdentity());
	TransformApplies.resize(iNumBones, true);

	// Ű���������� ��� ��� �� ��� ����
	Compute_CreateTransformShader(BoneMatrices, CurKeyFrames);
	
	// ��Ʈ �̵� ��
	_uint iRootTransBoneIndex = m_pBoneGroup->CRef_RootTransBoneInd();
	_uint iRootRotateBoneIndex = m_pBoneGroup->CRef_RootRotateBoneInd();
	_uint iRootScaleBoneIndex = m_pBoneGroup->CRef_RootScaleBoneInd();
	_float4x4 TempTransform[3] = { IdenetityFloat4x4, IdenetityFloat4x4, IdenetityFloat4x4 };
	_float4x4 RootBoneTransform = IdenetityFloat4x4;


	// ������ ��� �� ��Ʈ�� ã�ư��
	const FBoneAnimData* pBoneAnimData = Find_BoneAnim(m_MainAnim.Cur.iAnimID);
	if (pBoneAnimData != nullptr)
	{
		const FBoneAnimChannelData* pTransBone = pBoneAnimData->Find_AnimChannelData(iRootTransBoneIndex);
		// ��Ʈ�� ��� �ʱ�ȭ �� �̵��� ����� ���� ����
		if (pTransBone != nullptr)
		{
			//TempTransform[0] = BoneMatrices[i];
		}
		const FBoneAnimChannelData* pRotateBone = pBoneAnimData->Find_AnimChannelData(iRootRotateBoneIndex);
		if (pRotateBone != nullptr)
		{
			//TempTransform[1] = BoneMatrices[i];
		}
		const FBoneAnimChannelData* pScaleBone = pBoneAnimData->Find_AnimChannelData(iRootScaleBoneIndex);
		if (pScaleBone != nullptr)
		{
			//TempTransform[2] = BoneMatrices[i];
		}
	}
	for (_uint i = 0; i < iNumBones; i++)
	{
		const FBoneAnimChannelData* pBoneAnimChannelData = pBoneAnimData->Find_AnimChannelData(i);

		// ��� ���� �����
		if (nullptr != pBoneAnimChannelData)
		{
			
		}
		// ���� ��󿡼� ����
		else
		{
			BoneMatrices[i] = IdenetityFloat4x4;
			TransformApplies[i] = false;
		}
	}
	// ��Ʈ��� ����, �߰� ������ 0.01, ȸ�� Y 180��.
	RootBoneTransform = XMLoadFloat4x4(&TempTransform[0]) * XMLoadFloat4x4(&TempTransform[1]) 
						* XMLoadFloat4x4(&TempTransform[2]) * XMLoadFloat4x4(&m_pBoneGroup->CRef_BoneTransforms()[0]);

	// ��Ʈ��� ���� + ����
	_vector vSimPos = {}, vSimRot = {}, vSimScale = {};
	XMMatrixDecompose(&vSimScale, &vSimRot, &vSimPos, RootBoneTransform);
	m_vRootTrans = vSimPos;
	m_vRootTransDelta = Apply_TransDeltaByRootAnimType(m_MainAnim.Cur.eRootMotion_TransApplyType, m_vRootTrans - m_vRootTrans_Prev);
	m_vRootTrans_Prev = m_vRootTrans;


	// �� Ʈ�������� �ִϸ��̼� ����
	m_pBoneGroup->Invalidate_BoneTransforms(BoneMatrices, TransformApplies);
}
*/

void CAnimationComponent::Invalidate_AnimationWithMask(CAnimMaskComp* pAnimMaskComp)
{
	if (!m_pBoneGroup || !m_pAnimGroup)
		return;

	if (pAnimMaskComp == nullptr)
	{
		//Invalidate_Animation();
		return;
	}

	/*auto vecRemainWeights = pAnimMaskComp->Get_RemainWeights();
	_uint iNumMasks = pAnimMaskComp->Get_NumAnimMasks();
	_uint iNumBones = m_pBoneGroup->Get_BoneDatas_Count();*/

	//for (_uint i = 0; i < iNumMasks; i++)
	//{

	//	// ����ũ�� Ű�������� ����
	//	for (_uint j = 0; j < iNumBones; j++)
	//	{
	//		if (rAnimMask.fTransitionGauge < 1.f)
	//		{
	//			FAnimInterpolate AnimInter = {};
	//			AnimInter.iAnimID = rAnimMask.iAnimID;
	//			AnimInter.fTrackPos = rAnimMask.fCurTrackPos;
	//			AnimInter.fWeight = rAnimMask.fTransitionGauge;
	//			AnimInter.vecChannelIDs.reserve(iNumBoneMasks);
	//			for (_uint j = 0; j < iNumBoneMasks; j++)
	//			{
	//				_int iID = rAnimMask.vecBoneMasks[j] ? rAnimMask.vecBoneMasks[j] * j : -1;	// ����ũ�� ������ -1�� ����
	//				AnimInter.vecChannelIDs.push_back(iID);
	//			}

	//			FAnimInterpolate PrevAnimInter = AnimInter;
	//			PrevAnimInter.iAnimID = rAnimMask.iPrevAnimID;
	//			PrevAnimInter.fTrackPos = rAnimMask.fPrevTrackPos;
	//			PrevAnimInter.fWeight = 1.f - rAnimMask.fTransitionGauge;

	//			FAnimInterpolate ArrAnimInter[2] = {
	//				AnimInter, PrevAnimInter
	//			};

	//			// ����ũ �ִϸ��̼ǳ��� �����Ѵ�.
	//			m_pAnimGroup->Interpolated_Anims(vecKeyFrames.data(), vecKeyFrames.size(), ArrAnimInter, 2);

	//			vecKeyFrameInters[i].fWeight = fWeight;
	//			vecKeyFrameInters[i].KeyFrames[j] = vecKeyFrames[j];
	//		}
	//		// ������ KeyFrameInterpolate�� �� �ִϸ��̼����κ��� ���´�.
	//		else
	//		{
	//			FKeyFrame KeyFrame = m_pAnimGroup->Interpolated_Anim(rAnimMask.iAnimID, j, rAnimMask.fCurTrackPos);
	//			vecKeyFrameInters[i].fWeight = fWeight;
	//			vecKeyFrameInters[i].KeyFrames[j] = KeyFrame;
	//		}
	//	}
	//}
}

void CAnimationComponent::Set_MaskAnim(
	_uint iMaskIndex, const wstring& strAnimName, const wstring& strBasisBone
	, const vector<wstring>& strEndBoneNames
	, _uint iRecursive
	, _float fSpeedMultiply, _bool bIsLoop
	, _bool bIsTransition, _float fTransitionSpeed, _bool bReverse)
{
	if (iMaskIndex < 0 || iMaskIndex >= (_uint)m_MaskAnim.size())
		return;

	auto pBoneAnimData = m_pAnimGroup->Find_BoneAnim(strAnimName);
	if (pBoneAnimData == nullptr)
		return;

	auto& Mask = m_MaskAnim[iMaskIndex];

	if (Mask.Cur.iAnimID != pBoneAnimData->iID || !Mask.Prev.bIsLoop)
	{
		if (Mask.Cur.iAnimID != pBoneAnimData->iID)
		{
			Mask.Prev = Mask.Cur;
			bIsTransition ? Mask.fTransBlend.Reset() : Mask.fTransBlend.Set_Max();
			Mask.fTransSpeed = fTransitionSpeed;
		}

		Mask.Cur.iAnimID = pBoneAnimData->iID;
		Mask.Cur.strAnimName = WstrToStr(pBoneAnimData->strName);
		Mask.Cur.fDuration = pBoneAnimData->fDuration;
		Mask.Cur.fTickPerSeconds = pBoneAnimData->fTickPerSecond;
		Mask.Cur.fSpeedMultiply = fSpeedMultiply;
		Mask.Cur.bIsLoop = bIsLoop;
		Mask.Cur.bIsReverse = bReverse;
		Mask.Cur.fTrackPos = (!bReverse) ? 0.f : Mask.Cur.fDuration;
		Mask.Cur.fPrevTrackPos = Mask.Cur.fTrackPos;
		Mask.Cur.bIsTransition = bIsTransition;
		Mask.Cur.eRootMotion_TransApplyType = ERootAnimType::XYZ;

		Mask.Cur.strStartBoneName = strBasisBone;
		Mask.Cur.strEndBoneNames = strEndBoneNames;
		Mask.Cur.iRecursive = iRecursive;

		if (Mask.Prev.strStartBoneName.empty() && !strBasisBone.empty())
			Mask.Prev.strStartBoneName = strBasisBone;
		if (Mask.Prev.strEndBoneNames.empty() && !strEndBoneNames.empty())
			Mask.Prev.strEndBoneNames = strEndBoneNames;

		Mask.bUsing = true;
		Mask.bEndMasking = false;
		Mask.fFadeOutBlend.Reset();
		
		/*ResetTrigger_AnimNotifyByCurAnim();
		Set_AnimNameToNotify();*/
	}
}

void CAnimationComponent::Tick_MaskAnimTime(_cref_time fTimeDelta)
{
	for (_uint i = 0; i < (_uint)m_MaskAnim.size(); i++)
	{
		auto& Mask = m_MaskAnim[i];
		// ������� �ƴϸ� ���� �ʴ´�.
		if (!Mask.bUsing)
			return;

		Mask.Cur.fPrevTrackPos = Mask.Cur.fTrackPos;
		Mask.Cur.fTrackPos += Mask.Cur.fTickPerSeconds * Mask.Cur.fSpeedMultiply * fTimeDelta * (Mask.Cur.bIsReverse ? -1 : 1);

		if (Mask.Cur.fTrackPos > Mask.Cur.fDuration)
		{
			Mask.Cur.fTrackPos = Mask.Cur.fDuration;
			if (Mask.Cur.bIsLoop)
				Mask.Cur.fTrackPos = 0.f;
			else
			{
				Mask.Cur.fTrackPos = Mask.Cur.fDuration;
				Mask.bEndMasking = true;
				Mask.fFadeOutBlend.Reset();
			}
		}
	}
}

void CAnimationComponent::Exit_MaskAnim(_uint iIndex, _float fFadeOutSpeed)
{
	if (iIndex >= (_uint)m_MaskAnim.size())
		return;

	m_MaskAnim[iIndex].bEndMasking = true;
	m_MaskAnim[iIndex].fFadeOutBlend.Reset();
	m_MaskAnim[iIndex].fFadeOutSpeed = fFadeOutSpeed;
}

void CAnimationComponent::Set_ADD_Animation(_uint iIndex, const wstring& strAnimName
	, const wstring strBasisBone, const vector<wstring>& strEndBoneNames
	, _uint iRecursive, _float fSpeedMultiply, _float fWeight, _bool bReverse)
{
	if (iIndex < 0 || iIndex >= (_uint)m_AddAnims.size())
		return;

	auto pBoneAnimData = m_pAnimGroup->Find_BoneAnim(strAnimName);
	if (pBoneAnimData == nullptr)
		return;

	auto& AddAnim = m_AddAnims[iIndex];

	// Add �ִϸ��̼� ����
	AddAnim.Add.iAnimID = pBoneAnimData->iID;
	AddAnim.Add.strAnimName = WstrToStr(pBoneAnimData->strName);
	AddAnim.Add.fDuration = pBoneAnimData->fDuration;
	AddAnim.Add.fTickPerSeconds = pBoneAnimData->fTickPerSecond;
	AddAnim.Add.fSpeedMultiply = fSpeedMultiply;
	AddAnim.Add.bIsLoop = false;
	AddAnim.Add.bIsReverse = bReverse;
	AddAnim.Add.fTrackPos = (!bReverse) ? 0.f : AddAnim.Add.fDuration;
	AddAnim.Add.fPrevTrackPos = AddAnim.Add.fTrackPos;
	AddAnim.Add.bIsTransition = true;
	AddAnim.Add.eRootMotion_TransApplyType = ERootAnimType::XYZ;

	AddAnim.Add.iRecursive = iRecursive;
	AddAnim.Add.strStartBoneName = strBasisBone;
	AddAnim.Add.strEndBoneNames = strEndBoneNames;

	AddAnim.bUsing = true;
	AddAnim.fWeight = fWeight;
}

void CAnimationComponent::Set_ADD_AnimFinish(_uint iIndex)
{
	m_AddAnims[iIndex].Add.fTickPerSeconds = m_AddAnims[iIndex].Add.fDuration;
	m_AddAnims[iIndex].bUsing = false;
}

void CAnimationComponent::Tick_ADD_AnimTime(_cref_time fTimeDelta)
{
	// ������� �ƴϸ� ���� �ʴ´�.
	for (_uint i = 0; i < (_uint)m_AddAnims.size(); i++)
	{
		auto& AddAnim = m_AddAnims[i];

		if (!AddAnim.bUsing)
			return;

		AddAnim.Add.fPrevTrackPos = AddAnim.Add.fTrackPos;
		AddAnim.Add.fTrackPos += AddAnim.Add.fTickPerSeconds * AddAnim.Add.fSpeedMultiply * fTimeDelta * (AddAnim.Add.bIsReverse ? -1 : 1);

		if (AddAnim.Add.fTrackPos > AddAnim.Add.fDuration)
		{
			AddAnim.Add.fTrackPos = AddAnim.Add.fDuration;
			if (AddAnim.Add.bIsLoop)
				AddAnim.Add.fTrackPos = 0.f;
			else
			{
				AddAnim.Add.fTrackPos = AddAnim.Add.fDuration;
				AddAnim.bUsing = false;
			}
		}
	}
	
}


_vector CAnimationComponent::Apply_TransDeltaByRootAnimType(ERootAnimType eType, _fvector vTranslate)
{
	// �׽�Ʈ ��, XYZ ��� ����ǵ��� ������.
	_vector vResult = vTranslate;
	switch (eType)
	{
	case ERootAnimType::X:
		vResult.m128_f32[1] = 0.f;
		vResult.m128_f32[2] = 0.f;
		break;
	case ERootAnimType::Y:
		vResult.m128_f32[0] = 0.f;
		vResult.m128_f32[2] = 0.f;
		break;
	case ERootAnimType::Z:
		vResult.m128_f32[1] = 0.f;
		vResult.m128_f32[2] = 0.f;
		break;
	case ERootAnimType::XY:
		vResult.m128_f32[2] = 0.f;
		break;
	case ERootAnimType::YZ:
		vResult.m128_f32[0] = 0.f;
		break;
	case ERootAnimType::ZX:
		vResult.m128_f32[1] = 0.f;
		break;
	case ERootAnimType::XYZ:

		break;
	}

	return vResult;
}

HRESULT CAnimationComponent::Clone_NotifyFromPrototype(const wstring& strProtoTag)
{
	// ������Ÿ�����κ��� Ŭ�δ��Ͽ� ��Ƽ���� ������Ʈ�� �ʱ�ȭ�Ѵ�.
	auto pComponent = m_pGameInstance->Clone_Component(0, strProtoTag);
	if (nullptr == pComponent)
		RETURN_EFAIL;

	m_pNotifyComp = DynPtrCast<CAnimNotifyComp>(pComponent);

	return S_OK;
}

HRESULT CAnimationComponent::Regist_EventToNotify(
	const string& strAnimName, const string& strGroupName,
	const string& strNotifyName, FastDelegate0<> Event)
{
	if (nullptr == m_pNotifyComp)
		RETURN_EFAIL;

	return m_pNotifyComp->Regist_EventToNotify(strAnimName, strGroupName, strNotifyName, Event);
}

HRESULT CAnimationComponent::Regist_ModelCompToNotify(weak_ptr<CCommonModelComp> pModelComp)
{
	if (nullptr == m_pNotifyComp)
		RETURN_EFAIL;

	return m_pNotifyComp->Regist_ModelComponent(pModelComp);
}

void CAnimationComponent::Trigger_AnimNotifyByCurAnim(_cref_time fTrackPos)
{
	if (nullptr == m_pNotifyComp)
		return;

	m_pNotifyComp->Trigger_AnimNotifyByCurAnim(fTrackPos);
}

void CAnimationComponent::ResetTrigger_AnimNotifyByCurAnim()
{
	if (nullptr == m_pNotifyComp)
		return;

	m_pNotifyComp->ResetTrigger_AnimNotifyByCurAnim();
}

void CAnimationComponent::Set_AnimNameToNotify()
{
	if (nullptr == m_pNotifyComp)
		return;

	m_pNotifyComp->Set_CurrentAnimation(m_MainAnim.Cur.strAnimName);
}
