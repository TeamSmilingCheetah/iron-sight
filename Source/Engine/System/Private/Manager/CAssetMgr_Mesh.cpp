#include "pch.h"
#include "System/Public/Manager/CAssetMgr.h"

void CAssetMgr::CreateEngineMesh()
{
    Ptr<CMesh> pMesh = nullptr;

    // =========
    // PointMesh
    // =========
    Vtx v;
    UINT i = 0;

    pMesh = new CMesh(true);
    pMesh->Create(&v, 1, &i, 1);
    GetInst()->AddAsset(L"PointMesh", pMesh);

    // =========
    // Rect Mesh
    // 4 개의 정점의 위치 설계
    // 0 -- 1
    // | \  |
    // 3 -- 2
    // =========
    Vtx arrVtx[4] = {};
    arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
    arrVtx[0].vUV = Vec2(0.f, 0.f);
    arrVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
    arrVtx[0].vNormal = Vec3(0.f, 0.f, -1.f);
    arrVtx[0].vTangent = Vec3(1.f, 0.f, 0.f);
    arrVtx[0].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
    arrVtx[1].vUV = Vec2(1.f, 0.f);
    arrVtx[1].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
    arrVtx[1].vNormal = Vec3(0.f, 0.f, -1.f);
    arrVtx[1].vTangent = Vec3(1.f, 0.f, 0.f);
    arrVtx[1].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
    arrVtx[2].vUV = Vec2(1.f, 1.f);
    arrVtx[2].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
    arrVtx[2].vNormal = Vec3(0.f, 0.f, -1.f);
    arrVtx[2].vTangent = Vec3(1.f, 0.f, 0.f);
    arrVtx[2].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
    arrVtx[3].vUV = Vec2(0.f, 1.f);
    arrVtx[3].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrVtx[3].vNormal = Vec3(0.f, 0.f, -1.f);
    arrVtx[3].vTangent = Vec3(1.f, 0.f, 0.f);
    arrVtx[3].vBinormal = Vec3(0.f, -1.f, 0.f);

    // 인덱스
    UINT Idx[6] = {0, 2, 3, 2, 0, 1};

    // 클래스 or 구조체 + 가상함수
    const auto& info = typeid(CMesh);
    const char* pName = info.name();

    pMesh = new CMesh(true);
    pMesh->Create(arrVtx, 4, Idx, 6);
    GetInst()->AddAsset(L"RectMesh", pMesh);

    // ==============
    // RectMesh_Debug
    // 4 개의 정점의 위치 설계
    // 0 -- 1
    // | \  |
    // 3 -- 2
    // ==============
    UINT DebugRectIdx[5] = {0, 1, 2, 3, 0};

    pMesh = new CMesh(true);
    pMesh->Create(arrVtx, 4, DebugRectIdx, 5);
    GetInst()->AddAsset(L"RectMesh_Debug", pMesh);


    // ==========
    // CircleMesh
    // ==========
    vector<Vtx> vecVtx;
    vector<UINT> vecIdx;


    UINT Slice = 40;
    float Radius = 0.5f;
    float Theta = 0.f;

    // 중심점
    v.vPos = Vec3(0.f, 0.f, 0.f);
    v.vUV = Vec2(0.5f, 0.5f);
    vecVtx.push_back(v);

    for (UINT i = 0; i < Slice + 1; ++i)
    {
        v.vPos = Vec3(Radius * cosf(Theta), Radius * sinf(Theta), 0.f);
        v.vUV = Vec2(v.vPos.x + 0.5f, 1.f - (v.vPos.y + 0.5f));
        vecVtx.push_back(v);

        Theta += (XM_PI * 2.f) / Slice;
    }

    for (UINT i = 0; i < Slice; ++i)
    {
        vecIdx.push_back(0);
        vecIdx.push_back(i + 1);
        vecIdx.push_back(i + 2);
    }

    pMesh = new CMesh(true);
    pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(),
                  static_cast<UINT>(vecIdx.size()));
    GetInst()->AddAsset(L"CircleMesh", pMesh);

    // =================
    // CircleMesh_Debug
    // =================
    vecIdx.clear();
    for (size_t i = 0; i < vecVtx.size() - 1; ++i)
    {
        vecIdx.push_back(static_cast<UINT>(i + 1));
    }

    pMesh = new CMesh(true);
    pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(),
                  static_cast<UINT>(vecIdx.size()));
    GetInst()->AddAsset(L"CircleMesh_Debug", pMesh);

    vecVtx.clear();
    vecIdx.clear();

    // ========
    // CubeMesh
    // ========
    // 24개의 정점이 필요
    // 평면 하나당 정점 4개 x 6면 = 24개
    Vtx arrCube[24] = {};

    // 윗면
    arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
    arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    arrCube[0].vUV = Vec2(0.f, 0.f);
    arrCube[0].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);
    arrCube[0].vBinormal = Vec3(0.f, 0.f, -1.f);

    arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
    arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    arrCube[1].vUV = Vec2(1.f, 0.f);
    arrCube[1].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);
    arrCube[1].vBinormal = Vec3(0.f, 0.f, -1.f);

    arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
    arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    arrCube[2].vUV = Vec2(1.f, 1.f);
    arrCube[2].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);
    arrCube[2].vBinormal = Vec3(0.f, 0.f, -1.f);

    arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
    arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    arrCube[3].vUV = Vec2(0.f, 1.f);
    arrCube[3].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);
    arrCube[3].vBinormal = Vec3(0.f, 0.f, -1.f);


    // 아랫 면
    arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
    arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
    arrCube[4].vUV = Vec2(0.f, 0.f);
    arrCube[4].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);
    arrCube[4].vBinormal = Vec3(0.f, 0.f, 1.f);

    arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
    arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
    arrCube[5].vUV = Vec2(1.f, 0.f);
    arrCube[5].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);
    arrCube[5].vBinormal = Vec3(0.f, 0.f, 1.f);

    arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
    arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
    arrCube[6].vUV = Vec2(1.f, 1.f);
    arrCube[6].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);
    arrCube[6].vBinormal = Vec3(0.f, 0.f, 1.f);

    arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
    arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
    arrCube[7].vUV = Vec2(0.f, 1.f);
    arrCube[7].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);
    arrCube[7].vBinormal = Vec3(0.f, 0.f, 1.f);

    // 왼쪽 면
    arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
    arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
    arrCube[8].vUV = Vec2(0.f, 0.f);
    arrCube[8].vTangent = Vec3(0.f, 0.f, -1.f);
    arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);
    arrCube[8].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
    arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
    arrCube[9].vUV = Vec2(1.f, 0.f);
    arrCube[9].vTangent = Vec3(0.f, 0.f, -1.f);
    arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);
    arrCube[9].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
    arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
    arrCube[10].vUV = Vec2(1.f, 1.f);
    arrCube[10].vTangent = Vec3(0.f, 0.f, -1.f);
    arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);
    arrCube[10].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
    arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
    arrCube[11].vUV = Vec2(0.f, 1.f);
    arrCube[11].vTangent = Vec3(0.f, 0.f, -1.f);
    arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);
    arrCube[11].vBinormal = Vec3(0.f, -1.f, 0.f);

    // 오른쪽 면
    arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
    arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
    arrCube[12].vUV = Vec2(0.f, 0.f);
    arrCube[12].vTangent = Vec3(0.f, 0.f, 1.f);
    arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);
    arrCube[12].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
    arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
    arrCube[13].vUV = Vec2(1.f, 0.f);
    arrCube[13].vTangent = Vec3(0.f, 0.f, 1.f);
    arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);
    arrCube[13].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
    arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
    arrCube[14].vUV = Vec2(1.f, 1.f);
    arrCube[14].vTangent = Vec3(0.f, 0.f, 1.f);
    arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);
    arrCube[14].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
    arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
    arrCube[15].vUV = Vec2(0.f, 1.f);
    arrCube[15].vTangent = Vec3(0.f, 0.f, 1.f);
    arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);
    arrCube[15].vBinormal = Vec3(0.f, -1.f, 0.f);

    // 뒷 면
    arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
    arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
    arrCube[16].vUV = Vec2(0.f, 0.f);
    arrCube[16].vTangent = Vec3(-1.f, 0.f, 0.f);
    arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);
    arrCube[16].vBinormal = Vec3(0.f, -1.f, 1.f);

    arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
    arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
    arrCube[17].vUV = Vec2(1.f, 0.f);
    arrCube[17].vTangent = Vec3(-1.f, 0.f, 0.f);
    arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);
    arrCube[17].vBinormal = Vec3(0.f, -1.f, 1.f);

    arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
    arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
    arrCube[18].vUV = Vec2(1.f, 1.f);
    arrCube[18].vTangent = Vec3(-1.f, 0.f, 0.f);
    arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);
    arrCube[18].vBinormal = Vec3(0.f, -1.f, 1.f);

    arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
    arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
    arrCube[19].vUV = Vec2(0.f, 1.f);
    arrCube[19].vTangent = Vec3(-1.f, 0.f, 0.f);
    arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);
    arrCube[19].vBinormal = Vec3(0.f, -1.f, 1.f);

    // 앞 면
    arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);
    arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrCube[20].vUV = Vec2(0.f, 0.f);
    arrCube[20].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);
    arrCube[20].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
    arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrCube[21].vUV = Vec2(1.f, 0.f);
    arrCube[21].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);
    arrCube[21].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
    arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrCube[22].vUV = Vec2(1.f, 1.f);
    arrCube[22].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);
    arrCube[22].vBinormal = Vec3(0.f, -1.f, 0.f);

    arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
    arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
    arrCube[23].vUV = Vec2(0.f, 1.f);
    arrCube[23].vTangent = Vec3(1.f, 0.f, 0.f);
    arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);
    arrCube[23].vBinormal = Vec3(0.f, -1.f, 0.f);

    // 인덱스
    for (int i = 0; i < 12; i += 2)
    {
        vecIdx.push_back(i * 2);
        vecIdx.push_back(i * 2 + 1);
        vecIdx.push_back(i * 2 + 2);

        vecIdx.push_back(i * 2);
        vecIdx.push_back(i * 2 + 2);
        vecIdx.push_back(i * 2 + 3);
    }

    pMesh = new CMesh(true);
    pMesh->Create(arrCube, 24, vecIdx.data(), static_cast<UINT>(vecIdx.size()));
    GetInst()->AddAsset(L"CubeMesh", pMesh);

    // ==============
    // CubeMesh_Debug
    // ==============
    vecIdx.clear();
    vecIdx = vector<UINT>{0, 1, 2, 3, 0, 7, 4, 5, 6, 7, 4, 3, 2, 5, 6, 1};

    pMesh = new CMesh(true);
    pMesh->Create(arrCube, 24, vecIdx.data(), static_cast<UINT>(vecIdx.size()));
    GetInst()->AddAsset(L"CubeMesh_Debug", pMesh);

    // ============
    // Sphere Mesh
    // ============
    vecVtx.clear();
    vecIdx.clear();

    float fRadius = 0.5f;

    // Top
    v.vPos = Vec3(0.f, fRadius, 0.f);
    v.vUV = Vec2(0.5f, 0.f);
    v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    v.vNormal = v.vPos;
    v.vNormal.Normalize();
    v.vTangent = Vec3(1.f, 0.f, 0.f);
    v.vBinormal = Vec3(0.f, 0.f, -1.f);
    vecVtx.push_back(v);

    // Body
    UINT iStackCount = 40; // 가로 분할 개수
    UINT iSliceCount = 40; // 세로 분할 개수

    float fStackAngle = XM_PI / iStackCount;
    float fSliceAngle = XM_2PI / iSliceCount;

    float fUVXStep = 1.f / static_cast<float>(iSliceCount);
    float fUVYStep = 1.f / static_cast<float>(iStackCount);

    for (UINT i = 1; i < iStackCount; ++i)
    {
        float phi = i * fStackAngle;

        for (UINT j = 0; j <= iSliceCount; ++j)
        {
            float theta = j * fSliceAngle;

            v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
                          , fRadius * cosf(i * fStackAngle)
                          , fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));

            v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
            v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
            v.vNormal = v.vPos;
            v.vNormal.Normalize();

            v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
            v.vTangent.y = 0.f;
            v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
            v.vTangent.Normalize();

            v.vNormal.Cross(v.vTangent, v.vBinormal);
            v.vBinormal.Normalize();

            vecVtx.push_back(v);
        }
    }

    // Bottom
    v.vPos = Vec3(0.f, -fRadius, 0.f);
    v.vUV = Vec2(0.5f, 1.f);
    v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
    v.vNormal = v.vPos;
    v.vNormal.Normalize();

    v.vTangent = Vec3(1.f, 0.f, 0.f);
    v.vBinormal = Vec3(0.f, 0.f, -1.f);
    vecVtx.push_back(v);

    // 인덱스
    // 북극점
    for (UINT i = 0; i < iSliceCount; ++i)
    {
        vecIdx.push_back(0);
        vecIdx.push_back(i + 2);
        vecIdx.push_back(i + 1);
    }

    // 몸통
    for (UINT i = 0; i < iStackCount - 2; ++i)
    {
        for (UINT j = 0; j < iSliceCount; ++j)
        {
            // +
            // | \
            // +--+
            vecIdx.push_back((iSliceCount + 1) * (i) + (j) + 1);
            vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
            vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j) + 1);

            // +--+
            //  \ |
            //    +
            vecIdx.push_back((iSliceCount + 1) * (i) + (j) + 1);
            vecIdx.push_back((iSliceCount + 1) * (i) + (j + 1) + 1);
            vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
        }
    }

    // 남극점
    UINT iBottomIdx = static_cast<UINT>(vecVtx.size()) - 1;
    for (UINT i = 0; i < iSliceCount; ++i)
    {
        vecIdx.push_back(iBottomIdx);
        vecIdx.push_back(iBottomIdx - (i + 2));
        vecIdx.push_back(iBottomIdx - (i + 1));
    }

    pMesh = new CMesh(true);
    pMesh->Create(vecVtx.data(), static_cast<UINT>(vecVtx.size()), vecIdx.data(),
                  static_cast<UINT>(vecIdx.size()));
    AddAsset(L"SphereMesh", pMesh);
    vecVtx.clear();
    vecIdx.clear();
}
