// ����� ��ġ�� ��� ���̴�

cbuffer CB : register(b0)
{
    uint g_iCount;
};

// �� ��� ����ü
// 64 BYTE
struct BonesBuffer
{
    float4x4 Transform;
};

struct BoneParent
{
    int iParentID;
};


groupshared int iCurIndex = 0;

// ������ �ִϸ��̼� �ΰ��� �޴´�.
StructuredBuffer<BonesBuffer> InputTransform : register(t0);
StructuredBuffer<BoneParent> InputHierarchy : register(t1); // 'InputHierarchy'�� ����
// ��� �ִϸ��̼�
RWStructuredBuffer<BonesBuffer> Output : register(u0);

[numthreads(1024, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    // ������ �׷� ������ �� �����尡 �ڽ��� �ε����� ����ϵ��� ��
    uint iIndex = Gid.x * 64 + GTid.x;

    // ���� �������� �θ� ID ��������
    int iParentID = InputHierarchy[iIndex].iParentID;

    // �θ� ���Ǿ����� Ȯ���ϰ� ������ �ʾҴٸ� ���
    while (iParentID >= iCurIndex)
    {
        // �θ� ID �ٽ� Ȯ��
        iParentID = InputHierarchy[iIndex].iParentID;
    }
    // �����尡 ����ϴ� ���� �ٸ� �����忡�� ��� �纸
    GroupMemoryBarrierWithGroupSync();

    // ���� �������� ��ȯ ��� ��������
    float4x4 currentTransform = InputTransform[iIndex].Transform;

    // �θ��� ��ȯ ��İ� ���� �������� ��ȯ ����� ����
    float4x4 resultTransform = mul(currentTransform, Output[iParentID].Transform);

    // ��� ����� ��� ���ۿ� ����
    Output[iIndex].Transform = resultTransform;
}
