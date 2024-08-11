// �� �ִϸ��̼��� �����ϴ� ��� ���̴�

cbuffer CB : register(b0)
{
    uint g_iSize;           // ������ �迭 ũ��
	float g_fBlend;			// ���� ����ġ
	float2 g_fPadding;
};





// �� ��� ����ü
// 64 BYTE
struct BonesBuffer
{
	float3 vPosition;
	float4 vRotation;
	float3 vScale;
};



// ������ �ִϸ��̼� �ΰ��� �޴´�.
StructuredBuffer<BonesBuffer> InputBlend0 : register(t0);
StructuredBuffer<BonesBuffer> InputBlend1 : register(t1);
// ��� �ִϸ��̼�
RWStructuredBuffer<BonesBuffer> Output : register(u0);


float4 slerp(float4 q1, float4 q2, float t)
{
    // ���ʹϾ� ���� ���ϱ�
    float dotProduct = dot(q1, q2);

    // ���ʹϾ� ������ ������ 180�� �̻��̸�, �ϳ��� ť�ʹϾ��� ����
    if (dotProduct < 0.0f)
    {
        q2 = -q2;
        dotProduct = -dotProduct;
    }

    // t�� ����
    float scale0 = 1.0f - t;
    float scale1 = t;

    // ���ʹϾ� ����
    if ((1.0f - dotProduct) > 0.001f)
    {
        float angle = acos(dotProduct);
        float invSinTheta = 1.0f / sin(angle);
        scale0 = sin((1.0f - t) * angle) * invSinTheta;
        scale1 = sin(t * angle) * invSinTheta;
    }

    // ������ ť�ʹϾ��� ��ȯ�մϴ�.
    return scale0 * q1 + scale1 * q2;
}


[numthreads(1024, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    uint iIndex = GI;
    if (iIndex < g_iSize)
    {
        Output[iIndex].vPosition = lerp(InputBlend1[iIndex].vPosition, InputBlend0[iIndex].vPosition, g_fBlend);
        Output[iIndex].vRotation = slerp(InputBlend1[iIndex].vRotation, InputBlend0[iIndex].vRotation, g_fBlend);
        Output[iIndex].vScale = lerp(InputBlend1[iIndex].vScale, InputBlend0[iIndex].vScale, g_fBlend);
    }
}