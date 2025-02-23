#pragma once
#include "CEntity.h"


class CConstBuffer :
    public CEntity
{
private:
    ComPtr<ID3D11Buffer>		m_CB;
    D3D11_BUFFER_DESC           m_Desc;
    const CB_TYPE               m_Type;

public:
    int Create(UINT _BufferSize);
    void SetData(void* _pData, UINT _DataSize = 0);

    void Binding();
    void Binding_CS();

public:
    CLONE_DISABLE(CConstBuffer);
    CConstBuffer(CB_TYPE _Type);
    ~CConstBuffer();
};

