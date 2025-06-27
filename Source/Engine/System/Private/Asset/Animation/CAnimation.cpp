#include "pch.h"
#include "System/Public/Asset/Animation/CAnimation.h"
#include "System/Public/Asset/Animation/CSkeleton.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "System/Public/Rendering/Tool/FBX/CFBXLoader.h"

CAnimation::CAnimation(bool _bEngineRes)
	: CAsset(ANIMATION, _bEngineRes)
	, m_Skeleton(nullptr)
	, m_StartFrame(0)
	, m_EndFrame(0)
	, m_FrameLength(0)
	, m_StartTime(0.)
	, m_EndTime(0.)
	, m_TimeLength(0.)
	, m_TimeMode(FbxTime::EMode::eFrames30)
	, m_BoneFrameData(nullptr)
{
}

CAnimation::CAnimation(const CAnimation& _Src)
	: CAsset(ANIMATION, false)	// FIXME : clone은 engine resource가 아니라고 가정.
	, m_Skeleton(_Src.m_Skeleton)
	, m_StartFrame(_Src.m_StartFrame)
	, m_EndFrame(_Src.m_EndFrame)
	, m_FrameLength(_Src.m_FrameLength)
	, m_StartTime(_Src.m_StartTime)
	, m_EndTime(_Src.m_EndTime)
	, m_TimeLength(_Src.m_TimeLength)
	, m_TimeMode(_Src.m_TimeMode)
	, m_BoneFrameData(nullptr)
	, m_vecKeyFrames(_Src.m_vecKeyFrames)	// FIXME : cpu 데이터는 날려야 함. 나중에 gpu copyresource로 변경하기
{
	CreateBoneFrameSB();
}

CAnimation::~CAnimation()
{
	if (nullptr != m_BoneFrameData)
		delete m_BoneFrameData;
}

void CAnimation::Crop(int _StartIdx, int _EndIdx)
{
	// TODO : CPU 데이터를 GPU에서 가져오도록 변경
	m_FrameLength = _EndIdx - _StartIdx + 1;

	assert(m_TimeMode == FbxTime::EMode::eFrames30);
	m_TimeLength = m_FrameLength / 30.0;

	UINT boneCount = m_Skeleton->GetBoneCount();

	vector<tFrameTrans> croppedKeyFrames(m_FrameLength * boneCount);

	for (int i = _StartIdx; i <= _EndIdx; ++i)
	{
		for (UINT j = 0; j < boneCount; ++j)
		{
			croppedKeyFrames[(i - _StartIdx) * boneCount + j] = m_vecKeyFrames[i * boneCount + j];
		}
	}

	m_vecKeyFrames = std::move(croppedKeyFrames);

	CreateBoneFrameSB();
}

vector<Ptr<CAnimation>> CAnimation::LoadFromFBX(CFBXLoader& _loader)
{
	vector<Ptr<CAnimation>> vecRetClips;

	Ptr<CAnimation> pAnim = nullptr;

	const vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();
	const vector<tBone*>& vecBones = _loader.GetBones();

	if (vecBones.empty())
		return vecRetClips;

	for (int clipIdx = 0; clipIdx < static_cast<int>(vecAnimClip.size()); ++clipIdx)
	{
		pAnim = new CAnimation;
		pAnim->SetName(vecAnimClip[clipIdx]->strName + L".anim");
		pAnim->m_Skeleton = CAssetMgr::GetInst()->Load<CSkeleton>(L"Skeleton\\" + vecBones[0]->strBoneName + L".bone");
		pAnim->m_StartFrame = static_cast<int>(vecAnimClip[clipIdx]->tStartTime.GetFrameCount(
			vecAnimClip[clipIdx]->eMode));
		pAnim->m_EndFrame = static_cast<int>(vecAnimClip[clipIdx]->tEndTime.GetFrameCount(
			vecAnimClip[clipIdx]->eMode));
		pAnim->m_FrameLength = pAnim->m_EndFrame - pAnim->m_StartFrame;

		pAnim->m_StartTime = vecAnimClip[clipIdx]->tStartTime.GetSecondDouble();
		pAnim->m_EndTime = vecAnimClip[clipIdx]->tEndTime.GetSecondDouble();
		pAnim->m_TimeLength = pAnim->m_EndTime - pAnim->m_StartTime;

		pAnim->m_TimeMode = vecAnimClip[clipIdx]->eMode;

		pAnim->m_vecKeyFrames.resize(vecBones.size() * pAnim->m_FrameLength);

		for (size_t boneIdx = 0; boneIdx < vecBones.size(); ++boneIdx)
		{
			if (vecBones[boneIdx]->vecKeyFrame.empty())
				continue;

			for (size_t frameIdx = 0; frameIdx < vecBones[boneIdx]->vecKeyFrame[clipIdx].size(); ++frameIdx)
			{
				Vec4 vTranslate{}, vScale{}, qRot{};
				vTranslate = GetVectorFromFbxVector(vecBones[boneIdx]->vecKeyFrame[clipIdx][frameIdx].matTransform.GetT());
				vScale = GetVectorFromFbxVector(vecBones[boneIdx]->vecKeyFrame[clipIdx][frameIdx].matTransform.GetS());
				qRot = GetVectorFromFbxVector(vecBones[boneIdx]->vecKeyFrame[clipIdx][frameIdx].matTransform.GetQ());

				// 같은 frame끼리 정보를 모아 둠
				pAnim->m_vecKeyFrames[static_cast<UINT>(vecBones.size()) * frameIdx + boneIdx]
					= tFrameTrans{vTranslate, vScale, qRot};
			}
		}

		// structuredbuffer 만들어두기
		pAnim->CreateBoneFrameSB();
		//pAnim->m_BoneFrameData = new CStructuredBuffer;
		//pAnim->m_BoneFrameData->Create(sizeof(tFrameTrans)
		//    , static_cast<UINT>(vecBones.size()) * pAnim->m_FrameLength
		//    , SRV_ONLY, false, pAnim->m_vecKeyFrames.data());


		wstring strFilePath = L"Animation\\" + pAnim->GetName();

		// AssetMgr 등록 (key 값 설정)
		CAssetMgr::GetInst()->AddAsset<CAnimation>(strFilePath, pAnim);

		// Save (relative path 설정)
		pAnim->Save(strFilePath);

		// Return 벡터에 추가
		vecRetClips.push_back(pAnim);
	}

	return vecRetClips;
}

int CAnimation::Save(const wstring& _RelativePath)
{
	wstring strRelativePath = CPathMgr::GetInst()->MakeFileName(_RelativePath);
	SetRelativePath(strRelativePath);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + strRelativePath;

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	assert(pFile);

	// 키값, 상대 경로
	SaveWString(GetName(), pFile);
	SaveWString(GetKey(), pFile);
	SaveWString(GetRelativePath(), pFile);

	// skeleton reference 저장
	SaveAssetRef(m_Skeleton, pFile);

	// Animation 정보
	fwrite(&m_StartFrame, sizeof(int), 1, pFile);
	fwrite(&m_EndFrame, sizeof(int), 1, pFile);
	fwrite(&m_FrameLength, sizeof(int), 1, pFile);
	fwrite(&m_TimeMode, sizeof(FbxTime::EMode), 1, pFile);
	fwrite(&m_StartTime, sizeof(double), 1, pFile);
	fwrite(&m_EndTime, sizeof(double), 1, pFile);
	fwrite(&m_TimeLength, sizeof(double), 1, pFile);

	// Keyframe 정보
	fwrite(m_vecKeyFrames.data(), sizeof(tFrameTrans), static_cast<size_t>(m_Skeleton->GetBoneCount() * m_FrameLength), pFile);

	fclose(pFile);

	return S_OK;
}

int CAnimation::Load(const wstring& _strFilePath)
{
	// 읽기모드로 파일열기
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// 키값, 상대경로
	wstring strName, strKey, strRelativePath;
	LoadWString(strName, pFile);
	LoadWString(strKey, pFile);
	LoadWString(strRelativePath, pFile);

	SetName(strName);
	SetKey(strKey);
	SetRelativePath(strRelativePath);

	// skeleton reference 로드
	LoadAssetRef(m_Skeleton, pFile);

	// Animation 정보
	fread(&m_StartFrame, sizeof(int), 1, pFile);
	fread(&m_EndFrame, sizeof(int), 1, pFile);
	fread(&m_FrameLength, sizeof(int), 1, pFile);
	fread(&m_TimeMode, sizeof(FbxTime::EMode), 1, pFile);
	fread(&m_StartTime, sizeof(double), 1, pFile);
	fread(&m_EndTime, sizeof(double), 1, pFile);
	fread(&m_TimeLength, sizeof(double), 1, pFile);

	// Keyframe 정보
	m_vecKeyFrames.resize(m_Skeleton->GetBoneCount() * m_FrameLength);
	for (int i = 0; i < m_vecKeyFrames.size(); ++i)
		fread(&m_vecKeyFrames[i], sizeof(tFrameTrans), 1, pFile);

	// Structured Buffer 생성
	CreateBoneFrameSB();

	fclose(pFile);

	return S_OK;
}

void CAnimation::CreateBoneFrameSB()
{
	if (m_BoneFrameData != nullptr)
		delete m_BoneFrameData;

	m_BoneFrameData = new CStructuredBuffer;
	m_BoneFrameData->Create(sizeof(tFrameTrans)
		, m_Skeleton->GetBoneCount() * m_FrameLength
		, SRV_ONLY, false, m_vecKeyFrames.data());
}
