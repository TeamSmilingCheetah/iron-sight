#ifndef SOURCE_ENGINE_SHADER_MESH_COLLISION_
#define SOURCE_ENGINE_SHADER_MESH_COLLISION_

#include "struct.fx"
#include "value.fx"

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
 * @param LeftNormal, RightNormal, Depth [OUT] 삼각형 평면들의 Normal값과 침투 깊이
 * @return 충돌 여부
 */
bool TrianglesIntersect(float3 A0, float3 A1, float3 A2, float3 B0, float3 B1, float3 B2,
    out float3 LeftNormal, out float3 RightNormal, out float Depth)
{
    float3 AEdge[3] = { A1 - A0, A2 - A1, A0 - A2 };
    float3 BEdge[3] = { B1 - B0, B2 - B1, B0 - B2 };
    float3 ANormal = normalize(cross(AEdge[0], -AEdge[2]));
    float3 BNormal = normalize(cross(BEdge[0], -BEdge[2]));

    // Axis Check
    if (Separated(ANormal, A0, A1, A2, B0, B1, B2))
    {
        return false;
    }
    if (Separated(BNormal, A0, A1, A2, B0, B1, B2))
    {
        return false;
    }

    [unroll]
    for (int i = 0; i < 3; ++i) {
        [unroll]
        for (int j = 0; j < 3; ++j) {
            float3 axis = cross(AEdge[i], BEdge[j]);
            if (Separated(axis, A0, A1, A2, B0, B1, B2))
            {
                return false;
            }
        }
    }

    // 통과했다면 충돌
    LeftNormal = ANormal;
    RightNormal = BNormal;

    // Center 간의 거리로 깊이 측정값 처리
    float3 ACenter = (A0 + A1 + A2) / 3.0f;
    float3 BCenter = (B0 + B1 + B2) / 3.0f;
    float3 CenterToCenter = BCenter - ACenter;
    float3 AverageNormal = normalize(ANormal + BNormal);
    Depth = abs(dot(CenterToCenter, AverageNormal));

    return true;
}

[numthreads(64, 1, 1)]
void CS_MeshCollisionBatch(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint TaskIndex = DispatchThreadID.x;

    // Limit Bound
    uint TaskNumber, Stride;
    CollisionTasks.GetDimensions(TaskNumber, Stride);

    if (TaskIndex >= TaskNumber)
    {
        return;
    }

    CollisionResult LocalResult;
    LocalResult.Collided = 0;
    LocalResult.PenetrationDepth = 0.f;
    LocalResult.LeftNormal = float3(0, 0, 0);
    LocalResult.RightNormal = float3(0, 0, 0);

    CollisionTask Task = CollisionTasks[TaskIndex];

    // 모든 삼각형 쌍에 대한 충돌 검사 진행
    for (uint i = 0; i < Task.LeftTriCount; ++i)
    {
        uint LeftIndex0 = AllIndices[Task.LeftIndexOffset + i * 3 + 0];
        uint LeftIndex1 = AllIndices[Task.LeftIndexOffset + i * 3 + 1];
        uint LeftIndex2 = AllIndices[Task.LeftIndexOffset + i * 3 + 2];

        float3 LeftVertex0 = AllVertices[Task.LeftVertexOffset + LeftIndex0];
        float3 LeftVertex1 = AllVertices[Task.LeftVertexOffset + LeftIndex1];
        float3 LeftVertex2 = AllVertices[Task.LeftVertexOffset + LeftIndex2];

        for (uint j = 0; j < Task.RightTriCount; ++j)
        {
            uint RightIndex0 = AllIndices[Task.RightIndexOffset + j * 3 + 0];
            uint RightIndex1 = AllIndices[Task.RightIndexOffset + j * 3 + 1];
            uint RightIndex2 = AllIndices[Task.RightIndexOffset + j * 3 + 2];

            float3 RightVertex0 = AllVertices[Task.RightVertexOffset + RightIndex0];
            float3 RightVertex1 = AllVertices[Task.RightVertexOffset + RightIndex1];
            float3 RightVertex2 = AllVertices[Task.RightVertexOffset + RightIndex2];

            // Check Collision
            float3 LeftNormal, RightNormal;
            float Depth;
            if (TrianglesIntersect(LeftVertex0, LeftVertex1, LeftVertex2, RightVertex0, RightVertex1, RightVertex2, LeftNormal, RightNormal, Depth))
            {
                // 제일 깊은 Depth를 가진 결과로 갱신
                if (Depth > LocalResult.PenetrationDepth)
                {
                    LocalResult.Collided = 1;
                    LocalResult.LeftNormal = LeftNormal;
                    LocalResult.RightNormal = RightNormal;
                    LocalResult.PenetrationDepth = Depth;
                }
            }
        }
    }

    Results[TaskIndex] = LocalResult;
}

#endif // SOURCE_ENGINE_SHADER_MESH_COLLISION_
