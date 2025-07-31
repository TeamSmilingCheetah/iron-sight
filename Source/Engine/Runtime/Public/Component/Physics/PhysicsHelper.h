#pragma once

#include "Engine/System/Public/Asset/Mesh/CMesh.h"

extern "C" {
#include "qhull/libqhull_r/qhull_ra.h"
}

/**
 * @brief 물리 관련 계산을 도와주는 유틸리티 클래스
 * qhull 라이브러리를 래핑하여 Convex Hull을 생성하는 기능 제공
 */
class FPhysicsHelper
{
public:
	static Ptr<CMesh> CreateConvexHullFromMesh(Ptr<CMesh> InOriginalMesh);
};

/**
 * @brief qhull 라이브러리의 qhT 구조체에 대한 RAII 래퍼 클래스
 * C Style 메모리 할당, 해제를 고려하여 안전하게 리소스를 해제할 수 있도록 구성
 * @param QHullData qhull 핵심 데이터 타입으로 모든 상태 정보, 정점, 면, 옵션 등의 데이터를 전부 포함
 * @param Initialized 리소스 할당 여부를 확인할 수 있는 플래그
 */
class QhullManager
{
private:
	qhT QHullData;
	bool Initialized;

public:
	QhullManager();
	~QhullManager();

	// Getter & Setter
	qhT* GetQHullData() { return &QHullData; }
	void SetInitialized() { Initialized = true; }
};
