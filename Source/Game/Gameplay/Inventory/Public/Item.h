#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

enum class ITEM_TYPE
{
	WEAPON,					// 무기

	FIRST_AID_KIT,			// 의료용 키트
	MED_KIT,				// 구급 상자
	BANDAGE,				// 붕대
	ADRENALINE_SYRINGE,		// 아드레날린 주사
	PAIN_KILLER,			// 진통제
	ENERGY_DRINK,			// 에너지 드링크

	GRENADE,				// 수류탄
	MOLOTOV,				// 화염병
	SMOKEBOMB,				// 연막탄

	AMMO_9,					// 9mm
	AMMO_5,					// 5.56mm
	AMMO_7,					// 7.62mm

	END,
};

class ItemScript
	: public CScript
{
private:
	UINT			m_Count;		// 아이템 개수
	ITEM_TYPE		m_ItemType;		// 아이템 종류 (총알, 치료 아이템, 투척 무기, 부착물 등)
	Ptr<CTexture>	m_ItemImage;	// 아이템 이미지
	// float			m_Weight;		// 무게

public:
	ITEM_TYPE GetItemType() const { return m_ItemType; }
	void SetItemType(ITEM_TYPE _Type) { m_ItemType = _Type; }

	UINT GetCount() const { return m_Count; }
	void SetCount(UINT _Count) { m_Count = _Count; }

	void IncrementCount() { ++m_Count; }
	void DecrementCount() { --m_Count; }

	Ptr<CTexture> GetImage() const { return m_ItemImage; }
	void SetImage(Ptr<CTexture> _Image) { m_ItemImage = _Image; }


public:
	virtual void Init() override;
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

	virtual void BeginOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) override;
	virtual void Overlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) override;
	virtual void EndOverlap(class CCollider3D* _Collider, CGameObject* _OtherObject, CCollider3D* _OtherCollider) override;


public:
	CLONE(ItemScript);
	ItemScript();
	~ItemScript();
};
