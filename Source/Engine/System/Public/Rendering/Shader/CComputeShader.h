#pragma once
#include "Engine/System/Public/Rendering/Shader/CShader.h"

/**
 * @brief Compute Shader를 관리하는 클래스
 * HLSL Compute Shader의 생성, 실행, 바인딩, 그룹 계산 등을 담당
 *
 * @var m_CSBlob HLSL Compile Result (ID3DBlob) 저장
 * @var m_CS Compute Shader 객체 (ID3D11ComputeShader)
 * @var m_Const Material 상수 버퍼
 * @var m_GroupPerThreadX X축 스레드당 그룹 수 (고정값)
 * @var m_GroupPerThreadY Y축 스레드당 그룹 수 (고정값)
 * @var m_GroupPerThreadZ Z축 스레드당 그룹 수 (고정값)
 * @var m_GroupX X축 그룹 개수(실행 시 계산)
 * @var m_GroupY Y축 그룹 개수(실행 시 계산)
 * @var m_GroupZ Z축 그룹 개수(실행 시 계산)
 */
class CComputeShader :
	public CShader
{
private:
	ComPtr<ID3DBlob> m_CSBlob;
	ComPtr<ID3D11ComputeShader> m_CS;

protected:
	MtrlConst m_Const;

	const int m_GroupPerThreadX;
	const int m_GroupPerThreadY;
	const int m_GroupPerThreadZ;

	int m_GroupX;
	int m_GroupY;
	int m_GroupZ;

private:
	int CreateComputeShader(const wstring& PBlobFilePath,
	                        const wstring& PEffectsFilePath, const wstring& PEntryPointName);

public:
	int Execute();
	virtual int Binding() = 0;
	virtual void CalcGroupCount() = 0;
	virtual void Clear() = 0;

	// Special Member Function
	CLONE_DISABLE(CComputeShader);
	CComputeShader(const wstring& PBlobPath, const wstring& PRelativeFilePath, const wstring& PFunctionName,
	               int PGroupPerX, int PGroupPerY, int PGroupPerZ);
	~CComputeShader() override;
};
