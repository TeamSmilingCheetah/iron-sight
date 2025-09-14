# Iron-Sight
> DirectX 11 기반 3D FPS 게임 엔진 및 데모 프로젝트

## 목차

<details>
<summary> Click Here </summary>

> [0. 프로젝트 개요](#0-프로젝트-개요)
>
> [1. 주요 특징](#1-주요-특징)
>
> [2. 기술 사양](#2-기술-사양)
>
> [3. 주요 기능](#3-주요-기능)
>
> [4. 아키텍처 구조](#4-아키텍처-구조)
>
> [5. 데모 플레이](#5-데모-플레이)
>
> [6. 세부 개발 과정](#6-세부-개발-과정)

</details>

### 0. 프로젝트 개요

Iron-Sight는 DirectX 11을 기반으로 개발된 3D FPS 프로젝트로,
DX11 API 및 C++을 사용한 역할에 맞게 3가지로 구분된  솔루션의 형태로 개발되었습니다.
FPS 장르에 기반한, 클라이언트에서 제공할 수 있는 다양한 요소들을 체험할 수 있습니다.

- 장르: 3D FPS
- 지원 플랫폼: Window
- 개발 기간: 2025년 04월 ~

#### 🧑‍💻 Contributors

<table>
  <tr>
    <td align="center"><a href="https://github.com/TraceofLight"><img src="https://avatars.githubusercontent.com/u/98262849?v=4?s=100" width="100px;" alt=""/><br /><sub><b>김희준</b></sub></a></td>
    <td align="center"><a href="https://github.com/ySsio"><img src="https://avatars.githubusercontent.com/u/88093363?v=4?s=100" width="100px;" alt=""/><br /><sub><b>유승헌</b></sub></a></td>
    <td align="center"><a href="https://github.com/Han1West"><img src="https://avatars.githubusercontent.com/u/117280459?v=4?s=100" width="100px;" alt=""/><br /><sub><b>한원서</b></sub></a></td>
    <td align="center"><a href="https://github.com/1231245151"><img src="https://avatars.githubusercontent.com/u/160561239?v=4?s=100" width="100px;" alt=""/><br /><sub><b>홍민기</b></sub></a></td>
  </tr>
</table>

#### 🛠️ 기술 스택

| 카테고리                | 기술                  |
|---------------------|---------------------|
| **Graphics API**    | DirectX 11          |
| **Language**        | C++17, HLSL         |
| **Physics**         | Custom 3D Physics   |
| **Asset Pipeline**  | FBX SDK, DirectXTex |
| **IDE**             | VS 2022, Rider 25.1 |
| **Version Control** | Git (Asset Excluded) |

---

### 1. 주요 특징

- 전투 시스템
- 시각 효과
- AI 교전 상호작용

---

### 2. 기술 특징

- **Deferred Rendering Pipeline** - 다중 광원 환경 최적화
- **Component-Based Architecture** - 유연한 게임 오브젝트 시스템
- **Advanced Physics System** - 정밀한 충돌 감지 및 물리 시뮬레이션
- **FBX Asset Pipeline** - 완전한 3D 모델 임포트 지원

---

### 3. 주요 기능

#### 🎮 FPS 게임플레이 시스템
- **3인칭/1인칭 카메라 전환** - 동적 시점 변경
- **물리 기반 이동** - 가속도/감속도 적용된 자연스러운 움직임
- **무기 시스템** - 반동 패턴, 탄도학 시뮬레이션
- **AI 시스템** - 시야각 기반 적 감지 및 추적

#### 🎨 렌더링 엔진
- **Deferred Shading** - 다중 광원 효율적 처리
- **PBR Materials** - IBL 기반 Unreal PBR 구현
- **Dynamic Lighting** - 실시간 그림자 및 조명
<!--- **Post-Processing** - HDR, Bloom, Anti-Aliasing -->

#### 🌍 지형 시스템
- **GPU Heightmaps** - 컴퓨트 셰이더 기반 지형 생성
- **Mesh Collider** - 메쉬 기반의 지형 구축
- **Real-time Editing** - 런타임 지형 편집 도구
- **LOD System** - 거리 기반 상세도 조절

#### ⚡ 성능 최적화
- **Frustum Culling** - 시야 외부 객체 제거
- **Batch Rendering** - 드로우 콜 최소화
- **Object Pooling** - 메모리 할당 최적화
- **Component Caching** - 스크립트 참조 캐싱

---

### 4. 아키텍처 구조

// TO BE DISPLAYED

---

### 5. 데모 플레이

#### 🎮 시연 영상

![Image](https://github.com/user-attachments/assets/b30df659-5bed-4f69-8816-57034a398eea)

#### 🚀 중점 시연 항목
- ✅ **FPS 플레이어 컨트롤** - WASD 이동, 마우스 조준
- ✅ **무기 시스템** - 총기 발사, 반동 효과
- ✅ **AI 적 시스템** - 플레이어 추적 및 공격

---

### 6. 세부 개발 과정

[<img src="https://img.shields.io/badge/notion-D3D3D3?style=for-the-badge&logo=notion&logoColor=black"/>](https://traceoflight.notion.site/project-iron-sight)

---

## Acknowledgments

This project uses the Qhull library to Use Convex Hull Collision

**Qhull**
* Copyright (c) 1993-2020 C.B. Barber and The Geometry Center
* The Qhull license is available in the [LICENSE-Qhull.txt](LICENSE-Qhull.txt) file.


<!--
## 🔧 빌드 및 실행

### 시스템 요구사항
- **OS**: Windows 10/11 (64-bit)
- **GPU**: DirectX 11 지원 그래픽 카드
- **RAM**: 8GB 이상 권장
- **Visual Studio**: 2019 이상

### 빌드 방법
```bash
git clone https://github.com/username/iron-sight.git
cd iron-sight
# Visual Studio에서 Iron-Sight.sln 열기
# Release/Debug 모드 선택 후 빌드
```
-->

<!--
#### 📈 개발 과정 및 학습 성과

### 해결한 주요 기술적 과제
1. **매 프레임 객체 검색 최적화** - Begin() 캐싱으로 성능 90% 향상
2. **지형 충돌 지터링 해결** - 히스테리시스 적용으로 안정성 확보
3. **스크립트 다형성 구현** - 템플릿 기반 타입 시스템 구축
4. **FBX 파이프라인 구축** - 복잡한 3D 모델 완전 지원

### 적용한 현업 기법
- **Component-Entity-System** 패턴
- **RAII** 기반 리소스 관리
- **Template Metaprogramming** 활용
- **GPU Compute Shader** 최적화
-->
