#include "pch.h"
#include "CClearColorCS.h"

CClearColorCS::CClearColorCS()
    : CComputeShader(L"Shader\\test.fx", "CS_ClearTexture", 32, 32, 1)
{
}

CClearColorCS::~CClearColorCS()
{
}

int CClearColorCS::Binding()
{
    if (nullptr == m_TargetTex)
        return E_FAIL;

    // ��ĥ�� Texture �� U0 �� ���ε�
    m_TargetTex->Binding_UAV_CS(0);

    // �ؽ��� �ػ� ���� ����
    m_Const.iArr[0] = static_cast<int>(m_TargetTex->GetWidth());
    m_Const.iArr[1] = static_cast<int>(m_TargetTex->GetHeight());

    // ��ĥ�� �������� ����
    m_Const.v4Arr[0] = m_ClearColor;

    return S_OK;
}

void CClearColorCS::CalcGroupCount()
{
    m_GroupX = m_TargetTex->GetWidth() / m_GroupPerThreadX;
    m_GroupY = m_TargetTex->GetHeight() / m_GroupPerThreadY;
    m_GroupZ = 1;

    if (m_TargetTex->GetWidth() % m_GroupPerThreadX)
        m_GroupX += 1;

    if (m_TargetTex->GetHeight() % m_GroupPerThreadY)
        m_GroupY += 1;
}

void CClearColorCS::Clear()
{
    m_TargetTex->Clear_UAV_CS();
    m_TargetTex = nullptr;
}
