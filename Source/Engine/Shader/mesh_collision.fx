#ifndef SOURCE_ENGINE_SHADER_MESH_COLLISION_
#define SOURCE_ENGINE_SHADER_MESH_COLLISION_

#include "struct.fx"
#include "value.fx"
#define MAX_COLLISION_COUNT 4096

/**
 * @brief 특정 축에 삼각형을 투영하여 최대, 최소값을 반환
 * 임의의 축에 대해, 면을 정사영한 1차원 좌표값을 반환한다고 볼 수 있음
 */
float2 Project(float3 axis, float3 v0, float3 v1, float3 v2) {
    float d0 = dot(v0, axis);
    float d1 = dot(v1, axis);
    float d2 = dot(v2, axis);
    return float2(min(d0, min(d1, d2)), max(d0, max(d1, d2)));
}

/**
 * @brief 특정 축에서 두 삼각형이 분리되는지 검사
 * 구간 검사를 통해 겹치는 부분이 없다면 특정 축에 대해 두 면은 겹치지 않는다
 */
bool Separated(float3 axis, float3 A0, float3 A1, float3 A2, float3 B0, float3 B1, float3 B2) {
    // Early Return
    if (dot(axis, axis) < 1e-9f)
    {
        return false;
    }

    // 정사영된 값들의 충돌 체크
    float2 ProjectionA = Project(axis, A0, A1, A2);
    float2 ProjectionB = Project(axis, B0, B1, B2);
    return (ProjectionA.y < ProjectionB.x) || (ProjectionB.y < ProjectionA.x);
}

/**
 * @brief 삼각형 - 삼각형 충돌 판정 함수
 * Separating Axis Theorem을 활용한 판정 처리
 * @param A0, A1, A2 [IN] 삼각형 A의 꼭지점 좌표
 * @param B0, B1, B2 [IN] 삼각형 B의 꼭지점 좌표
 * @param penetrationDepth [OUT] 침투 깊이
 * @return 충돌 여부
 */
bool TrianglesIntersect(float3 A0, float3 A1, float3 A2, float3 B0, float3 B1, float3 B2, out float penetrationDepth)
{
    float3 AEdge[3] = { A1 - A0, A2 - A1, A0 - A2 };
    float3 BEdge[3] = { B1 - B0, B2 - B1, B0 - B2 };
    float3 ANormal = normalize(cross(AEdge[0], -AEdge[2]));
    float3 BNormal = normalize(cross(BEdge[0], -BEdge[2]));

    // Axis Check
    if (Separated(ANormal, A0, A1, A2, B0, B1, B2))
    {
        penetrationDepth = 0.0f;
        return false;
    }
    if (Separated(BNormal, A0, A1, A2, B0, B1, B2))
    {
        penetrationDepth = 0.0f;
        return false;
    }

    // Edge Check
    [unroll]
    for (int i = 0; i < 3; ++i) {
        [unroll]
        for (int j = 0; j < 3; ++j) {
            float3 axis = cross(AEdge[i], BEdge[j]);
            if (Separated(axis, A0, A1, A2, B0, B1, B2))
            {
                penetrationDepth = 0.0f;
                return false;
            }
        }
    }

    // Calculate Depth
    // 두 삼각형의 중심점 사이의 거리를 기반으로 계산
    float3 ACenter = (A0 + A1 + A2) / 3.0f;
    float3 BCenter = (B0 + B1 + B2) / 3.0f;
    float3 CenterToCenter = BCenter - ACenter;

    // Calculate Normal Direction Distance
    // 범위 제한 적용
    float3 AvgNormal = normalize(ANormal + BNormal);
    float RawPenetration = abs(dot(CenterToCenter, AvgNormal));
    penetrationDepth = min(RawPenetration * 0.05f, 10.0f);

    return true;
}

/**
 * @brief Shader Entrypoint, Mesh 2개의 충돌을 연산하고, 충돌 결과를 반환한다
 * @param DTid
 */
[numthreads(32, 32, 1)]
void CS_MeshCollision(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= LeftTriCount || DTid.y >= RightTriCount)
        return;

    uint3 LeftIdx = LeftIndices[DTid.x];
    float3 L0 = LeftVertices[LeftIdx.x].Pos;
    float3 L1 = LeftVertices[LeftIdx.y].Pos;
    float3 L2 = LeftVertices[LeftIdx.z].Pos;

    uint3 RightIdx = RightIndices[DTid.y];
    float3 R0 = RightVertices[RightIdx.x].Pos;
    float3 R1 = RightVertices[RightIdx.y].Pos;
    float3 R2 = RightVertices[RightIdx.z].Pos;

    float PenetrationDepth;
    if (TrianglesIntersect(L0, L1, L2, R0, R1, R2, PenetrationDepth))
    {
        uint Idx;
        InterlockedAdd(CollisionCount[0], 1, Idx);
        if (Idx < MAX_COLLISION_COUNT)
        {
            Results[Idx].LeftNormal = normalize(cross(L1 - L0, L2 - L0));
            Results[Idx].RightNormal = normalize(cross(R1 - R0, R2 - R0));
            Results[Idx].PenetrationDepth = PenetrationDepth;
        }
    }
}

#endif // SOURCE_ENGINE_SHADER_MESH_COLLISION_
