// �� �ִϸ��̼��� �����ϴ� ��� ���̴�


cbuffer CB : register(b0)
{
    uint g_iSize; // ������ �迭 ũ��
    float3 g_fPadding;
};

// �� ��� ����ü
// 64 BYTE
struct KeyFrameBuffer
{
    float3 vPosition;
    float4 vRotation;
    float3 vScale;
};

struct TransformBuffer
{
    float4x4 Transform;
};


// ������ 
StructuredBuffer<KeyFrameBuffer> InputKeyframes : register(t0);
// ��� Ʈ������
RWStructuredBuffer<TransformBuffer> OutputMatrices : register(u0);


float4x4 CreateTransform(float3 Position, float4 Quat, float3 Scale)
{
    // ���ʹϾ��� ȸ�� ��ķ� ��ȯ
    float4x4 RotationMatrix = float4x4(
        1.0f - 2.0f * (Quat.y * Quat.y + Quat.z * Quat.z), 2.0f * (Quat.x * Quat.y - Quat.w * Quat.z), 2.0f * (Quat.x * Quat.z + Quat.w * Quat.y), 0.0f,
        2.0f * (Quat.x * Quat.y + Quat.w * Quat.z), 1.0f - 2.0f * (Quat.x * Quat.x + Quat.z * Quat.z), 2.0f * (Quat.y * Quat.z - Quat.w * Quat.x), 0.0f,
        2.0f * (Quat.x * Quat.z - Quat.w * Quat.y), 2.0f * (Quat.y * Quat.z + Quat.w * Quat.x), 1.0f - 2.0f * (Quat.x * Quat.x + Quat.y * Quat.y), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // ��ġ, ȸ��, �������� �����Ͽ� ��ȯ ��� ����
    float4x4 Transform = float4x4(
        Scale.x * RotationMatrix[0],
        Scale.y * RotationMatrix[1],
        Scale.z * RotationMatrix[2],
        float4(Position, 1.f)
    );

    return Transform;
}


[numthreads(1024, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    uint iIndex = GI;    
    if (iIndex < g_iSize)
    {
        OutputMatrices[iIndex].Transform = CreateTransform(InputKeyframes[iIndex].vPosition,
                                                        InputKeyframes[iIndex].vRotation,
                                                        InputKeyframes[iIndex].vScale);
        OutputMatrices[iIndex].Transform = transpose(OutputMatrices[iIndex].Transform);
    }
    //OutputMatrices[iIndex].Transform = float4x4(
    //    Gid.x, Gid.y, Gid.z, 0.f,
    //    DTid.x, DTid.y, DTid.z, 0.f,
    //    GTid.x, GTid.y, GTid.z, 0.f,
    //    GI, GI, GI, GI
    //);
}