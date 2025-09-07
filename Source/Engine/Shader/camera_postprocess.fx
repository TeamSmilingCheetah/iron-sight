#ifndef _CAMERAPROCESS
#define _CAMERAPROCESS

#include "value.fx"
#include "func.fx"

// 필요 상수 버퍼
#define FadeInfo g_int_0            // 페이드 활성화 (0: 비활성, 1: 페이드 아웃, 2: 페이드 인)
#define HPLow g_int_1               // HP 낮음 신호
#define DamageEffect g_int_2        // 피격 효과 활성화 (0: 비활성, 1: 활성)
#define UseTexture g_btex_0          // 텍스쳐 사용 여부
#define FadeStart g_float_0         // 페이드 시작 시간
#define DamageTime g_float_1        // 피격 시간
#define EffectDuration g_float_2    // 효과 지속 시간 (초)
#define Color g_vec4_0
#define DamageColor g_vec4_1        // 피격 효과 색상 (r, g, b, a)
#define DamageTexture g_tex_0       // 피격 효과 텍스쳐


struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_CameraPostProcess(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_CameraPostProcess(VS_OUT _in) : SV_Target
{
    // 페이드 효과 계산 (기존 코드 유지)
    if (FadeInfo != 0)
    {
        float elapsedTime = g_Time - FadeStart;
        
        // 1초에 걸쳐 페이드
        float fadeAmount = saturate(elapsedTime);
        
        // 2번이면 역으로 밝아짐
        if (FadeInfo == 2)
            fadeAmount = 1.f - fadeAmount;
            
        return float4(0.f, 0.f, 0.f, fadeAmount);
    }


        // 피격 효과
    if (DamageEffect >= 1)
    {
        float elapsedTime = g_Time - DamageTime;
        
        // 효과 지속 시간 체크
        if (elapsedTime < EffectDuration + 0.001f)
        {
            // 화면 중심에서의 거리 계산 (비네팅 효과)
            float2 centerDist = abs(_in.vUV - 0.5f) * 2.0f;
            float borderDist = max(centerDist.x, centerDist.y);
            
            // 테두리 영역 설정 (더 넓은 영역)
            float outerEdge = 1.0f;
            float innerEdge = 0.6f; // HP 낮음보다 더 넓은 영역
            
            if (borderDist > innerEdge)
            {
                // 시간에 따른 페이드 아웃 계산
                float fadeProgress = elapsedTime / EffectDuration;
                float fadeAlpha = 1.0f - fadeProgress;
                
                // 테두리의 알파값 계산 (바깥쪽으로 갈수록 진해짐)
                float borderAlpha = smoothstep(innerEdge, outerEdge, borderDist);
                
                // 최종 알파값 = 테두리 알파 * 페이드 알파
                float finalAlpha = borderAlpha * fadeAlpha;

                // 해당 구문은 테스트용
                //if (finalAlpha < 0.1f)
                //    return float4(0.0f, 1.0f, 0.0f, 1.0f);
                //else
                //    return float4(0.0f, 0.0f, 1.0f, 1.0f);

                // 텍스쳐 사용 여부에 따른 색상 계산
                if (UseTexture)
                {
                    // 텍스쳐가 있으면 텍스쳐 샘플링
                    float4 texColor = DamageTexture.Sample(g_sam_0, _in.vUV);
                    
                    // 텍스쳐 색상 + 설정된 색상의 알파값 사용
                    float4 finalColor = float4(texColor.rgb, texColor.a * DamageColor.a);
                    finalColor.a *= finalAlpha;
                    
                    return finalColor;
                }
                else
                {
                    // 텍스쳐가 없으면 설정된 색상 사용
                    float4 finalColor = DamageColor;
                    finalColor.a *= finalAlpha;
                    return finalColor;
                }
            }
        }
    }
   
    // HP 낮음 효과 (기존 코드 유지)
    if (HPLow == 1)
    {
        // HP가 낮을 때 테두리 효과 계산
        float2 centerDist = abs(_in.vUV - 0.5f) * 2.0f;
        float borderDist = max(centerDist.x, centerDist.y);
        
        // 테두리 영역 계산
        float outerEdge = 1.0f;
        float innerEdge = 1.0f - 0.15f;
        
        if (borderDist > innerEdge)
        {
            float pulseAlpha = 0.2f + (sin(g_Time * 3.0f) + 1.0f) * 0.25f;
            
            // 테두리의 알파값 계산 (바깥쪽으로 갈수록 투명해짐)
            float alpha = smoothstep(innerEdge, outerEdge, borderDist);
            // HP가 낮을수록 더 진한 붉은색
            return float4(1.0f, 0.0f, 0.0f, alpha * pulseAlpha);
        }
    }
   
    
    // 효과가 없는 경우 픽셀 무시
    discard;


    float4 output = Color;
    return output;
}

#endif
