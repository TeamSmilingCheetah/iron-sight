#ifndef _FUNC
#define _FUNC

#include "value.fx"
#include "struct.fx"

float4 GetDebugColor(float2 _UV, int _Level)
{
    float2 vTexPos = _UV * _Level;

    int Col = floor(vTexPos.x);
    int Row = floor(vTexPos.y);

    // 홀수
    if (Col % 2)
    {
        if (Row % 2)
            return float4(1.f, 0.f, 1.f, 1.f);
        else
            return float4(0.f, 0.f, 0.f, 1.f);
    }
    // 짝수
    else
    {
        if (Row % 2)
            return float4(0.f, 0.f, 0.f, 1.f);
        else
            return float4(1.f, 0.f, 1.f, 1.f);
    }
}

void Phong2D(int _LightIdx, float3 _WorldPos, inout float3 _LightColor)
{
    float DistRatio = 1.f;

    // 광원처리
    if (g_Light2DInfo[_LightIdx].Type == 0) // DirLight
    {
        _LightColor += g_Light2DInfo[_LightIdx].vColor;
    }
    else if (g_Light2DInfo[_LightIdx].Type == 1) // PointLight
    {
        float Dist = distance(_WorldPos.xy, g_Light2DInfo[_LightIdx].vWorldPos.xy);
        if (Dist <= g_Light2DInfo[_LightIdx].Radius)
        {
            DistRatio = saturate(1.f - (Dist / g_Light2DInfo[_LightIdx].Radius));
            //DistRatio = saturate(cos(saturate((Dist / g_Light2DInfo[_LightIdx].Radius)) * (PI / 2.f)));

            _LightColor += g_Light2DInfo[_LightIdx].vColor * DistRatio;
        }
    }

    else // SpotLight
    {

    }
}

void Phong3D(int _LightIdx, float3 _ViewPos, float3 _ViewNormal
               , inout float3 _LightColor, inout float3 _vSpecPow)
{
    tLight3DInfo Light = g_Light3DInfo[_LightIdx];

    float3 vLightPow = (float3) 0.f;    // 물체가 광원으로부터 받는 빛의 세기
    float3 SpecPow = (float3) 0.f;      // 물체로부터 반사되어서 카메라로 들어가는 반사광의 크기
    float fDistRatio = 1.f;             // 거리에 따른 빛의 세기
    float3 vLight = (float3) 0.f;       // 광원의 진행 방향

    // Directional Light
    if (0 == Light.Type)
    {
        // 광원의 월드 방향
        vLight = normalize(Light.vDir);
        vLight = normalize(mul(float4(vLight, 0.f), g_matView)).xyz;
    }

    // Point Light
    else if (1 == Light.Type)
    {
        // 광원의 중심위치에서 물체로 향하는 방향벡터
        float3 vLightViewPos = (mul(float4(Light.WorldPos, 1.f), g_matView)).xyz;
        vLight = _ViewPos - vLightViewPos;
        float fDist = length(vLight);
        vLight = normalize(vLight);

        // 거리에 따라서 Point Light 로부터 받는 빛의 세기
        fDistRatio = saturate(1.f - (fDist / Light.Radius));
    }

    // Spot Light
    else
    {

    }

    // 픽셀에서의 노말과 광원의 방향을 이용한 빛의 세기 계산
    vLightPow = saturate(dot(-vLight, _ViewNormal));

    // 표면에서 빛의 튕겨나가는 반사 방향 구하기
    float3 vReflect = 2.f * dot(-vLight, _ViewNormal) * _ViewNormal + vLight;
    vReflect = normalize(vReflect);

    // ViewSpace 에서 픽셀의 위치가 곧 원점(카메라) 에서 픽셀을 향하는 시선벡터
    float3 vEye = normalize(_ViewPos);

    // 시선 벡터와 반사벡터를 내적해서 반사광의 세기를 구함
    SpecPow = saturate(dot(-vEye, vReflect));
    SpecPow = pow(SpecPow, 20);

    // 빛이 물체에 닿는 세기를 반환
    _LightColor += (vLightPow * Light.info.vColor + Light.info.vAmbient) * fDistRatio;
    _vSpecPow += SpecPow * Light.info.SpecCoeff * Light.info.vColor * fDistRatio;
}


float3 GetRandom(in Texture2D _NoiseTexture, uint _ID, uint _maxId)
{
    float2 vUV = (float2) 0.f;

    vUV.x = ((float) _ID / (float) (_maxId - 1)) + g_Time_Engine * 0.01f;
    vUV.y = sin(vUV.x * 20 * PI) * 0.5f + g_Time_Engine * 0.1f;
    float3 vRandom = _NoiseTexture.SampleLevel(g_sam_0, vUV, 0).xyz;

    return vRandom;
}




int IntersectsRay(float3 _Pos[3], float3 _vStart, float3 _vDir
                  , out float3 _CrossPos, out uint _Dist)
{
    // 삼각형 표면 방향 벡터
    float3 Edge[2] = { (float3) 0.f, (float3) 0.f };
    Edge[0] = _Pos[1] - _Pos[0];
    Edge[1] = _Pos[2] - _Pos[0];

    // 삼각형에 수직방향인 법선(Normal) 벡터
    float3 Normal = normalize(cross(Edge[0], Edge[1]));

    // 삼각형 법선벡터와 Ray 의 Dir 을 내적
    // 광선에서 삼각형으로 향하는 수직벡터와, 광선의 방향벡터 사이의 cos 값
    float NdotD = -dot(Normal, _vDir);

    float3 vStoP0 = _vStart - _Pos[0];
    float VerticalDist = dot(Normal, vStoP0); // 광선을 지나는 한점에서 삼각형 평면으로의 수직 길이

    // 광선이 진행하는 방향으로, 삼각형을 포함하는 평면까지의 거리
    float RtoTriDist = VerticalDist / NdotD;

    // 해당거리가 음수다 = 광원은 대상으로 향한게 아니다.
    if (RtoTriDist < 0.f)
        return 0;

    // 광선이, 삼각형을 포함하는 평면을 지나는 교점
    float3 vCrossPoint = _vStart + RtoTriDist * _vDir;

    // 교점이 삼각형 내부인지 테스트
    float3 P0toCross = vCrossPoint - _Pos[0];

    float3 Full = cross(Edge[0], Edge[1]);
    float3 U = cross(Edge[0], P0toCross);
    float3 V = cross(Edge[1], P0toCross);

    // 직선과 삼각형 평면의 교점이 삼각형 1번과 2번 사이에 존재하는지 체크
    //      0
    //     /  \
    //    1 -- 2
    if (dot(U, Full) < 0.f || 0.f < dot(V, Full))
        return 0;

    // 교점이 삼각형 내부인지 체크
    if (length(Full) < length(U) + length(V))
        return 0;

    _CrossPos = vCrossPoint;
    _Dist = (uint) (RtoTriDist * 10000.f);  // 데이터 손실을 최소화 하기위한 작업

    return 1;
}

matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{
    return g_arrBoneMat[(g_iBoneCount * _iRowIdx) + _iBoneIdx];
}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
    , inout float4 _vWeight, inout float4 _vIndices
    , int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vTangent += (mul(float4(_vTangent, 0.f), matBone) * _vWeight[i]).xyz;
        info.vBinormal += (mul(float4(_vBinormal, 0.f), matBone) * _vWeight[i]).xyz;
        info.vNormal += (mul(float4(_vNormal, 0.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.vPos;
    _vTangent = normalize(info.vTangent);
    _vBinormal = normalize(info.vBinormal);
    _vNormal = normalize(info.vNormal);
}

#endif
