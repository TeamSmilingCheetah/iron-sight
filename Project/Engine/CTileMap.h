#pragma once
#include "CRenderComponent.h"

#include "CTexture.h"


struct tTileInfo
{
    Vec2 LeftTopUV;
    Vec2 SliceUV;
};


class CStructuredBuffer;

class CTileMap :
    public CRenderComponent
{
    // Ÿ�� ��� ����
    int m_Col;
    int m_Row;

    // Ÿ�� 1���� ũ��
    float m_TileSizeX;
    float m_TileSizeY;

    // Ÿ�� ��Ʋ��
    Ptr<CTexture> m_TileAtlas;

    // ��Ʋ�� �ؽ��Ŀ��� Ÿ�� 1���� �����ϴ� ũ��(Pixel)
    UINT m_TileAtlasSizeX;
    UINT m_TileAtlasSizeY;

    // ��Ʋ�� �ؽ��Ŀ��� Ÿ���� ���� ���� ��� ����
    UINT m_TileAtlasCol;
    UINT m_TileAtlasRow;


    vector<tTileInfo> m_vecTileInfo; // Ÿ�� ������  
    CStructuredBuffer* m_Buffer; // Ÿ�� �����͸� GPU �� ������ �� ��� �� ����


public:
    void SetTileColRow(UINT _Col, UINT _Row);
    void SetTileSize(float _TileSizeX, float _TileSizeY);
    void SetAtlasTexture(Ptr<CTexture> _Tex) { m_TileAtlas = _Tex; }
    void SetTileAtlasSize(UINT _X, UINT _Y);
    void SetTileAtlasColRow(UINT _Col, UINT _Row);

private:
    void CalcObjectScale();

public:
    void Init() override;

    void FinalTick() override
    {
    };
    void Render() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _FILE) override;

    CLONE(CTileMap);
    CTileMap();
    CTileMap(const CTileMap& _Origin);
    ~CTileMap() override;
};
