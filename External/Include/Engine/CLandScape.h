#pragma once
#include "CRenderComponent.h"

#include "CHeightMapCS.h"
#include "CWeightMapCS.h"
#include "CRaycastCS.h"

#include "CTexture.h"

struct tRaycastOut
{
    Vec2    Location;
    UINT    Distance;
    int     Success;
};

enum LANDSCAPE_MODE
{
    NONE,
    HEIGHTMAP,
    SPLATING,
};

struct tWeight8
{
    float arrWeight[8];
};

class CLandScape :
    public CRenderComponent
{
private:
    UINT                    m_FaceX;        // 지형 가로 면 개수
    UINT                    m_FaceZ;        // 지형 세로 면 개수

    Ptr<CTexture>           m_HeightMap;    // 지형에 적용시킬 높이맵
    Ptr<CHeightMapCS>       m_HeightMapCS;  // 높이맵 수정용 컴퓨트 쉐이더

    // Raycasting
    Ptr<CRaycastCS>         m_RaycastCS;
    CStructuredBuffer*      m_RaycastOut;   // 마우스 픽킹 위치
    tRaycastOut             m_Out;          // 마우스 픽킹 위치
               
    vector<Ptr<CTexture>>   m_vecBrush;
    Vec2                    m_BrushScale;   // 브러쉬 크기
    UINT                    m_BrushIdx;     // 브러쉬 인덱스

    // WeightMap
    Ptr<CTexture>           m_ColorTex;
    Ptr<CTexture>           m_NormalTex;
    CStructuredBuffer*      m_WeightMap;
    UINT                    m_WeightWidth;
    UINT                    m_WeightHeight;
    Ptr<CWeightMapCS>       m_WeightMapCS;
    int                     m_WeightIdx;

    // LandScape 모드
    LANDSCAPE_MODE          m_Mode;


public:
    void SetFace(UINT _X, UINT _Z);
    void SetHeightMapTexture(Ptr<CTexture> _HeightMap) { m_HeightMap = _HeightMap; }
    void CreateHeightMap(UINT _Width, UINT _Height);
    void AddBrushTexture(Ptr<CTexture> _BrushTex) { m_vecBrush.push_back(_BrushTex); }
    void SetColorTexture(Ptr<CTexture> _ArrTex);
    void SetNormalTexture(Ptr<CTexture> _ArrTex);

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

private:
    void Init();
    void CreateMesh();
    void CreateMaterial();
    void CreateComputeShader();

private:
    int Raycasting();

public:
    CLONE(CLandScape);
    CLandScape();
    ~CLandScape();
};

