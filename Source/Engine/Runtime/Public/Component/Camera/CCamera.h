#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

class CFrustum;

class CCamera :
	public CComponent
{
	CFrustum* m_Frustum;

	// 공통
	PROJ_TYPE m_ProjType; // 투영 방식
	float m_Far; // 시야 최대 거리
	float m_AspectRatio; // 투영 범위에 대한 종횡비, 가로 / 세로
	UINT m_LayerCheck; // 카메라가 찍을 레이어 비트 체크
	int m_Priority; // 카메라 우선순위, 0 : MainCamera, -1 : 미등록

	// 원근투영(Perspective)
	float m_FOV; // FOV(Field Of View) 시야각

	// 직교투영(Orthographic)
	float m_Width; // 직교투영 가로길이
	float m_Scale; // 직교투영 배율

	// 카메라가 바라보는 화면에서 마우스를 향하는 직선
	tRay m_Ray;

	// 변환행렬
	Matrix m_matView; // View 행렬
	Matrix m_matViewInv; // View 역행렬

	Matrix m_matProj; // Projection 행렬
	Matrix m_matProjInv; // Projection 역행렬


	// 물체 분류
	map<ULONG64, vector<tInstObj>>		m_mapInstGroup_D;	// Deferred
	map<ULONG64, vector<tInstObj>>		m_mapInstGroup_F;	// Foward ( Opaque, Mask )
	map<INT_PTR, vector<tInstObj>>		m_mapSingleObj;		// Single Object

	vector<CGameObject*>                m_vecDecal;         // Decal
	vector<CGameObject*>                m_vecTransparent;   // 투명, 반투명
	vector<CGameObject*>                m_vecEffect;
	vector<CGameObject*>                m_vecParticle;      // 투명, 반투명, 입자 타입
	vector<CGameObject*>                m_vecPostprocess;   // 후처리 오브젝트
	vector<CGameObject*>                m_vecUI;            // UI


	bool m_bCamActive;
public:
	void SetProjType(PROJ_TYPE _Type) { m_ProjType = _Type; }
	void SetWidth(float _Width) { m_Width = _Width; }
	void SetAspectRatio(float _AR) { m_AspectRatio = _AR; }
	void SetScale(float _Scale) { m_Scale = _Scale; }
	void SetFar(float _Far) { m_Far = _Far; }
	void SetFOV(float _FOV) { m_FOV = _FOV; }
	void SetPriority(int _Priority) { m_Priority = _Priority; }

	float GetFar() const { return m_Far; }
	float GetFOV() const { return m_FOV; }
	float GetWidth() const { return m_Width; }
	float GetAspectRatio() const { return m_AspectRatio; }
	int GetCameraPriority() const { return m_Priority; }
	float GetScale() const { return m_Scale; }
	PROJ_TYPE GetProjType() const { return m_ProjType; }

	const tRay& GetRay() { return m_Ray; }

	const Matrix& GetViewMat() { return m_matView; }
	const Matrix& GetViewInvMat() { return m_matViewInv; }
	const Matrix& GetProjMat() { return m_matProj; }
	const Matrix& GetProjInvMat() { return m_matProjInv; }

	void LayerCheck(int _LayerIdx);
	void LayerCheckAll() { m_LayerCheck = 0xffffffff; }
	void LayerCheckClear() { m_LayerCheck = 0; }
	UINT GetLayerCheck() const { return m_LayerCheck; }

	void Begin() override;
	void FinalTick() override;

	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _FILE) override;

	void SortObject();

	void render_deferred();
	void render_decal();

	void render_forward();

	void render_transparent();
	void render_effect();
	void render_particle();
	void render_postprocess();
	void render_ui();
	void render_clear();

	void SetActive(bool _Active) { m_bCamActive = _Active; }
	bool IsActvie() { return m_bCamActive; }

private:
	bool IsObjectInFrustum(const CGameObject* _Object);
	void CalcRay();

public:
	CLONE(CCamera);
	CCamera();
	CCamera(const CCamera& _Origin);
	~CCamera() override;
};
