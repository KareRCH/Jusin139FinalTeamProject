// �� Ʈ������ ����� �޾� 

cbuffer CB : register(b0)
{
    float g_fBlend; // ���� ����ġ
    float3 g_fPadding;
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

    // ť�ʹϾ� ������ ������ 180�� �̻��̸�, �ϳ��� ť�ʹϾ��� ����
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
    Output[GI].vPosition = lerp(InputBlend0[GI].vPosition, InputBlend1[GI].vPosition, g_fBlend);
    GroupMemoryBarrierWithGroupSync();
    Output[GI].vRotation = slerp(InputBlend0[GI].vRotation, InputBlend1[GI].vRotation, g_fBlend);
    GroupMemoryBarrierWithGroupSync();
    Output[GI].vScale = lerp(InputBlend0[GI].vScale, InputBlend1[GI].vScale, g_fBlend);
    GroupMemoryBarrierWithGroupSync();
}