#ifndef _TARGETPOST
#define _TARGETPOST

#include "value.fx"
#include "func.fx"

// ============================
// TargetPostShader
// mesh    : RectMesh
// g_int_0 : TargetID
// g_int_1 : IsParentID
// g_tex_0 : DataTex
// g_tex_1 : PositionTex
// ============================

// 다른 오브젝트에 가려진건지 판단하는 함수
// isParentCheck가 true면 부모 오브젝트 id검사해서 같은 부모면 x
bool IsObscuredOtherObject(float2 Position, uint targetID, bool isParentCheck)
{
    if (g_btex_1) // Position 텍스처가 있는 경우만
    {
        // 현재 위치의 뷰 스페이스 깊이 (z값)
        float currentDepth = g_tex_1.Load(int3(Position, 0)).z;
        
        // 현재 픽셀의 부모 ID 가져오기 (부모 체크인 경우에만 사용)
        uint currentParentID = 0;
        if (isParentCheck)
        {
            currentParentID = (uint) g_tex_0.Load(int3(Position, 0)).g;
        }
        
        // 주변 4방향 검사
        float2 offsets[4] =
        {
            float2(-1, 0), float2(1, 0),
            float2(0, -1), float2(0, 1)
        };
        
        for (int i = 0; i < 4; ++i)
        {
            float2 neighborPos = Position + offsets[i];
            
            // 주변 픽셀의 ID 확인
            uint neighborID_r = (uint) g_tex_0.Load(int3(neighborPos, 0)).r;
            uint neighborID_g = (uint) g_tex_0.Load(int3(neighborPos, 0)).g;
            
            bool isDifferentObject = false;
            
            if (isParentCheck)
            {
                // 다른 부모를 가진 오브젝트만 다른 오브젝트로 판단
                if (neighborID_g != 0 && neighborID_g != currentParentID)
                {
                    isDifferentObject = true;
                }
            }
            else
            {
                // 다른 오브젝트 ID를 가진 경우 다른 오브젝트로 판단
                if (neighborID_g != 0 && neighborID_g != targetID)
                {
                    isDifferentObject = true;
                }
            }
            
            // 다른 오브젝트가 더 앞에 있다면 가려진 것
            if (isDifferentObject)
            {
                float neighborDepth = g_tex_1.Load(int3(neighborPos, 0)).z;
                if (neighborDepth < currentDepth)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// Vertex Shader 
struct VS_IN
{
    // semantic
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_TargetPost(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 전체 위치에 2배해주면 NDC좌표를 화면 전부를 지정하게 된다.
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
        
    return output;
}

float4 PS_TargetPost(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    int targetID = g_int_0;
    
    if (g_btex_0)
    {
        // 부모인지 자식오브젝트id를 찾는건지 확인
        if (g_int_1)
        {
                    // 현재 픽셀의 ID 체크
            uint centerID = (uint) g_tex_0.Load(int3(_in.vPosition.xy, 0)).r;
        
            if (centerID == targetID)
            {

                // 현재 픽셀이 다른 오브젝트에 의해 가려져 있는지 확인
                if (IsObscuredOtherObject(_in.vPosition.xy, targetID, true))
                {
                    discard; // 가려진 픽셀은 처리하지 않음
                }
                
                // 주변 4방향의 픽셀 ID 샘플링
                uint leftID = (uint) g_tex_0.Load(int3(_in.vPosition.xy + float2(-1, 0), 0)).r;
                uint rightID = (uint) g_tex_0.Load(int3(_in.vPosition.xy + float2(1, 0), 0)).r;
                uint topID = (uint) g_tex_0.Load(int3(_in.vPosition.xy + float2(0, -1), 0)).r;
                uint bottomID = (uint) g_tex_0.Load(int3(_in.vPosition.xy + float2(0, 1), 0)).r;
            
                // 엣지 검출 - 주변 픽셀과 ID가 다른 경우
                if (leftID != targetID || rightID != targetID ||
                    topID != targetID || bottomID != targetID)
                {
                    vColor = float4(1.f, 0.f, 0.f, 1.f); // 빨간색 테두리
                }
                else
                {
                    discard; // 테두리가 아닌 내부 픽셀은 버림
                }
            }
            else
            {
                discard;
            }
        }
        else
        {
            // 현재 픽셀의 ID 체크
            uint centerID = (uint) g_tex_0.Load(int3(_in.vPosition.xy, 0)).g;
        
            if (centerID == targetID)
            {
                // 현재 픽셀이 다른 오브젝트에 의해 가려져 있는지 확인
                if (IsObscuredOtherObject(_in.vPosition.xy, targetID, false))
                {
                    discard; // 가려진 픽셀은 처리하지 않음
                }
                
                // 주변 4방향의 픽셀 ID 샘플링
                uint leftID = (uint) g_tex_0.Load(int3(_in.vPosition.xy + float2(-1, 0), 0)).g;
                uint rightID = (uint) g_tex_0.Load(int3(_in.vPosition.xy + float2(1, 0), 0)).g;
                uint topID = (uint) g_tex_0.Load(int3(_in.vPosition.xy + float2(0, -1), 0)).g;
                uint bottomID = (uint) g_tex_0.Load(int3(_in.vPosition.xy + float2(0, 1), 0)).g;
            
                // 엣지 검출 - 주변 픽셀과 ID가 다른 경우
                if (leftID != targetID || rightID != targetID ||
                    topID != targetID || bottomID != targetID)
                {
                    vColor = float4(1.f, 0.f, 0.f, 1.f); // 빨간색 테두리
                }
                else
                {
                    discard; // 테두리가 아닌 내부 픽셀은 버림
                }
            }
            else
            {
                discard;
            }
        }
        
    }
    
    if (targetID == 0)
        discard;
    
    return vColor;
}

#endif
