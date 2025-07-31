#include "pch.h"
#include "Engine/Runtime/Public/Component/Physics/PhysicsHelper.h"

extern "C" {
#include "qhull/libqhull_r/qhull_ra.h"
}

/**
 * @brief 원본 메시로부터 Convex Hull 메시를 생성하는 함수
 * @param InOriginalMesh 원본 Mesh
 * @return [성공] Convex Hull로 생성된 CMesh Ptr | [실패] nullptr
 */
Ptr<CMesh> FPhysicsHelper::CreateConvexHullFromMesh(Ptr<CMesh> InOriginalMesh)
{
	// Early Return
	if (!InOriginalMesh.Get() || InOriginalMesh->GetVertexCount() == 0)
	{
		return nullptr;
	}

	// 원본 Mesh의 정점 데이터를 qhull이 요구하는 형식으로 제공
	Vtx* OriginalVertices = static_cast<Vtx*>(InOriginalMesh->GetVtxSysMem());
	UINT VertexCount = InOriginalMesh->GetVertexCount();
	int Dimension = 3;

	vector<coordT> HullPoints(VertexCount * Dimension);
	for (UINT i = 0; i < VertexCount; ++i)
	{
		HullPoints[i * Dimension + 0] = static_cast<coordT>(OriginalVertices[i].vPos.x);
		HullPoints[i * Dimension + 1] = static_cast<coordT>(OriginalVertices[i].vPos.y);
		HullPoints[i * Dimension + 2] = static_cast<coordT>(OriginalVertices[i].vPos.z);
	}

	// Execute
	QhullManager QHullManager;
	QHULL_LIB_CHECK

	// qhull Option
	// QJ (Joggled Input) : 동일 평면 상의 점들로 인한 오류 방지
	// Qbb (Scale Last Coordinate) : 평면에 가까운 데이터의 정밀도 오류 방지
	char Flags[] = "qhull QJ Qbb";
	int Exitcode = qh_new_qhull(QHullManager.GetQHullData(), Dimension, VertexCount,
	                            HullPoints.data(), false, Flags, nullptr,stderr);

	// Failure Return
	if (Exitcode)
	{
		LOG_ERROR_F("[PhysicsHelper] qhull Failed To Generate Convex Hull. Exit code: {}", Exitcode);
		return nullptr;
	}

	// Success
	QHullManager.SetInitialized();

	// Get Data From qhull Output
	qhT* qh = QHullManager.GetQHullData();
	vector<Vtx> HullVertices;
	HullVertices.resize(qh->num_vertices);
	vertexT* vertex;
	int VertexIdx = 0;
	FORALLvertices
	{
		HullVertices[VertexIdx].vPos = Vec3(
			static_cast<float>(vertex->point[0]),
			static_cast<float>(vertex->point[1]),
			static_cast<float>(vertex->point[2])
		);
		vertex->id = VertexIdx;
		++VertexIdx;
	}

	vector<UINT> HullIndices;
	facetT* facet;
	vertexT** vertexp;
	FORALLfacets
	{
		if (!facet->simplicial) continue;
		FOREACHvertex_(facet->vertices)
		{
			HullIndices.push_back(vertex->id);
		}
	}

	// Make Mesh With Data
	Ptr<CMesh> ConvexHullMesh = new CMesh(true);
	ConvexHullMesh->Create(HullVertices.data(), static_cast<UINT>(HullVertices.size()),
	                       HullIndices.data(), static_cast<UINT>(HullIndices.size()));

	return ConvexHullMesh;
}

/**
 * @brief QHullManager의 생성자, qhT 구조체를 초기화하는 역할
 */
QhullManager::QhullManager()
	: Initialized(false)
{
	qh_zero(&QHullData, stderr);
}

/**
 * @brief QHullManager의 소멸자, Initialize가 성공적으로 진행된 상황에서만 메모리를 정리하여 의도하지 않은 동작을 배제할 수 있음
 */
QhullManager::~QhullManager()
{
	// Release Memory If Initialized
	if (Initialized)
	{
		qh_freeqhull(&QHullData, !qh_ALL);
		int CurrentLong, TotalLong;
		qh_memfreeshort(&QHullData, &CurrentLong, &TotalLong);
		if (CurrentLong || TotalLong)
		{
			LOG_WARNING_F("[PhysicsHelper] qhull Memory Leak Detected. CurrentLong: {}, TotalLong: {}",
						  CurrentLong, TotalLong);
		}
	}
}
