#pragma once

class FMultiRenderTarget;
class CMaterial;

/**
 * @brief 렌더링 단계를 하나로 정의하기 위한 클래스
 *
 * @param MTargetMRT 이 패스가 렌더링할 목표 MRT
 * @param MaterialVector 이 패스에서 사용할 Material 목록
 */
class IRenderPass
	: public CEntity
{
private:
	FMultiRenderTarget* TargetMRT;
	vector<Ptr<CMaterial>> MaterialVector;
	vector<Ptr<CMesh>> MeshVector;

public:
	virtual void Execute(FRenderPassParameters& InParams) = 0;
	void AddMaterial(const Ptr<CMaterial>& InMaterial) { MaterialVector.push_back(InMaterial); }
	void AddMesh(const Ptr<CMesh>& InMesh) { MeshVector.push_back(InMesh); }

	// Getter & Setter
	FMultiRenderTarget* GetTargetMRT() const { return TargetMRT; }
	Ptr<CMaterial> GetMaterial(int InIdx) const { return MaterialVector[InIdx]; }
	Ptr<CMesh> GetMesh(int InIdx) const { return MeshVector[InIdx]; }

	void SetTargetMRT(FMultiRenderTarget* InMRT) { TargetMRT = InMRT; }

	// Special Member Function
	IRenderPass() = default;
	~IRenderPass() override = default;
};
