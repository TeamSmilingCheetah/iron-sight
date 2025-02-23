#pragma once
#include "CRenderComponent.h"

#include "CHeightMapCS.h"
#include "CWeightMapCS.h"
#include "CRaycastCS.h"

#include "CTexture.h"

struct tRaycastOut
{
    Vec2 Location;
    UINT Distance;
    int Success;
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
    UINT m_FaceX; // ���� ���� �� ����
    UINT m_FaceZ; // ���� ���� �� ����

    Ptr<CTexture> m_HeightMap; // ������ �����ų ���̸�
    Ptr<CHeightMapCS> m_HeightMapCS; // ���̸� ������ ��ǻƮ ���̴�

    // Raycasting
    Ptr<CRaycastCS> m_RaycastCS;
    CStructuredBuffer* m_RaycastOut; // ���콺 ��ŷ ��ġ
    tRaycastOut m_Out; // ���콺 ��ŷ ��ġ

    vector<Ptr<CTexture>> m_vecBrush;
    Vec2 m_BrushScale; // �귯�� ũ��
    UINT m_BrushIdx; // �귯�� �ε���

    // WeightMap
    Ptr<CTexture> m_ColorTex;
    Ptr<CTexture> m_NormalTex;
    CStructuredBuffer* m_WeightMap;
    UINT m_WeightWidth;
    UINT m_WeightHeight;
    Ptr<CWeightMapCS> m_WeightMapCS;
    int m_WeightIdx;

    // LandScape ���
    LANDSCAPE_MODE m_Mode;

public:
    void SetFace(UINT _X, UINT _Z);
    void SetHeightMapTexture(Ptr<CTexture> _HeightMap) { m_HeightMap = _HeightMap; }
    void CreateHeightMap(UINT _Width, UINT _Height);
    void AddBrushTexture(Ptr<CTexture> _BrushTex) { m_vecBrush.push_back(_BrushTex); }
    void SetColorTexture(Ptr<CTexture> _ArrTex);
    void SetNormalTexture(Ptr<CTexture> _ArrTex);

    void FinalTick() override;
    void Render() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;

private:
    void Init() override;
    void CreateMesh();
    void CreateMaterial();
    void CreateComputeShader();

    int Raycasting();

public:
    CLONE(CLandScape);
    CLandScape();
    ~CLandScape() override;
};
