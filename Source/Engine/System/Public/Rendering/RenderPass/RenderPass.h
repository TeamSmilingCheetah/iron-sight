#pragma once

class FMultiRenderTarget;
class CMaterial;

/**
 * @brief 렌더링 단계를 하나로 정의하기 위한 클래스
 *
 * @param MTargetMRT 이 패스가 렌더링할 목표 MRT
 * @param MaterialVector 이 패스에서 사용할 Material 목록
 */
class FRenderPass
	: public CEntity
{
private:
	FMultiRenderTarget* TargetMRT;
	vector<Ptr<CMaterial>> MaterialVector;

public:
	virtual void Execute() = 0;
	void AddMaterial(const Ptr<CMaterial>& PMaterial) { MaterialVector.push_back(PMaterial); }

	// Getter & Setter
	void SetTargetMRT(FMultiRenderTarget* PMRT) { TargetMRT = PMRT; }
	FMultiRenderTarget* GetTargetMRT() const { return TargetMRT; }

	// Special Member Function
	FRenderPass();
	~FRenderPass() override;
};
