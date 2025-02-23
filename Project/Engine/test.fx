#ifndef _TEST
#define _TEST

#include "value.fx"

#define WIDTH           g_int_0
#define HEIGHT          g_int_1
#define CLEAR_COLOR     g_vec4_0

// UnorderedAccess
// Read Write
RWTexture2D<float4> g_Target : register(u0);

// Group 당 스레드 개수 지정
// - HLSL 5.0 기준 그룹당 최대 1024개의 스레드 지정 가능
[numthreads(32, 32, 1)]

// ComputeShader Semantic
// SV_GroupID           : 스레드가 속한 그룹의 ID(그룹 인덱스)
// SV_GroupThreadID     : 스레드가 속한 그룹 내에서 자신의 위치를 3차원 인덱스로 표시
// SV_GroupIndex        : SV_GroupThreadID 를 1차원 인덱스로 변환
// SV_DispatchThreadID  : 전체 모든 그룹을 통틀어서 모든 스레드 중 해당 스레드의 고유한 ID

void CS_ClearTexture(int3 _id : SV_DispatchThreadID)
{
    if (WIDTH <= _id.x || HEIGHT <= _id.y)
    {
        return;
    }
    
    g_Target[_id.xy] = CLEAR_COLOR;
}




#endif

