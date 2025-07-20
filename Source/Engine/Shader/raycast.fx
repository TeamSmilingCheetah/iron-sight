#ifndef SOURCE_ENGINE_SHADER_RAYCAST_
#define SOURCE_ENGINE_SHADER_RAYCAST_

#include "struct.fx"
#include "value.fx"

/**
 * @brief Möller–Trumbore 알고리즘을 사용한 Ray-Triangle 교차 판정 함수
 * @param RayOrigin [IN] Ray의 시작 지점
 * @param RayDirection [IN] Ray의 방향 벡터
 * @param v0, v1, v2 [IN] 삼각형의 세 꼭지점
 * @param Distance, Normal [Out] Ray 시작 지점과 삼각형 교차 지점 사이 거리 및 삼각형의 Normal
 * @return 교차 여부
 */
bool RayTriangleIntersect(float3 RayOrigin, float3 RayDirection, float3 v0, float3 v1, float3 v2,
                          out float Distance, out float3 Normal)
{
    const float EPSILON = 0.0000001;
    float3 Edge1 = v1 - v0;
    float3 Edge2 = v2 - v0;

    // Calculate Normal Output
    Normal = normalize(cross(Edge1, Edge2));

    // 평행 육면체 부피 연산을 통한 평행 체크
    float3 h = cross(RayDirection, Edge2);
    float a = dot(Edge1, h);

    // Parallel Check
    if (a > -EPSILON && a < EPSILON)
    {
        return false;
    }

    // 관통 지점을 중심으로 무게중심 연산을 처리했을 때
    // 삼각형 내부를 벗어나는 지표인지 확인
    float f = 1.0 / a;
    float3 s = RayOrigin - v0;
    float u = f * dot(s, h);

    if (u < 0.0 || u > 1.0)
    {
        return false;
    }

    float3 q = cross(s, Edge1);
    float v = f * dot(RayDirection, q);

    if (v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    // Calculate Distance
    Distance = f * dot(Edge2, q);

    if (Distance > EPSILON)
    {
        return true;
    }

    // 시작점 기준으로 Distance가 음수라면 Ray 반대 방향
    return false;
}

[numthreads(64, 1, 1)]
void CS_RaycastBatch(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint TaskIndex = dispatchThreadID.x;

    uint Tasks, Stride;
    RaycastTasks.GetDimensions(Tasks, Stride);

    if (TaskIndex >= Tasks)
    {
        return;
    }

    RaycastResult LocalResult;
    LocalResult.IsHit = 0;
    LocalResult.Distance = 3.402823466e+38F; // FLT_MAX
    LocalResult.HitNormal = float3(0, 0, 0);

    RaycastTask Task = RaycastTasks[TaskIndex];

    // 할당된 메쉬의 모든 삼각형을 순회
    for (uint i = 0; i < Task.TriCount; ++i)
    {
        uint idx0 = AllIndices[Task.IndexOffset + i * 3 + 0];
        uint idx1 = AllIndices[Task.IndexOffset + i * 3 + 1];
        uint idx2 = AllIndices[Task.IndexOffset + i * 3 + 2];

        float3 v0 = AllVertices[Task.VertexOffset + idx0];
        float3 v1 = AllVertices[Task.VertexOffset + idx1];
        float3 v2 = AllVertices[Task.VertexOffset + idx2];

        float TempDistance;
        float3 TempNormal;
        if (RayTriangleIntersect(Task.RayOrigin, Task.RayDirection, v0, v1, v2, TempDistance, TempNormal))
        {
            // 더 가까운 교차점을 찾은 경우 결과 갱신
            if (TempDistance < LocalResult.Distance)
            {
                LocalResult.IsHit = 1;
                LocalResult.Distance = TempDistance;
                LocalResult.HitNormal = TempNormal;
            }
        }
    }

    RaycastResults[TaskIndex] = LocalResult;
}

#endif // SOURCE_ENGINE_SHADER_RAYCAST_
