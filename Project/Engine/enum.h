#pragma once

// ConstBuffer
enum class CB_TYPE
{
    TRANSFORM,
    MATERIAL,
    SPRITE,
    GLOBAL,
    END,
};


// RasterizerState Type
enum class RS_TYPE
{
    CULL_BACK, // �޸� �ø� ���
    CULL_FRONT, // �ո� �ø� ���
    CULL_NONE, // �ø����� ����
    WIRE_FRAME, // WireFrame ���
    END,
};


// BlendState
enum class BS_TYPE
{
    DEFAULT, // �������
    ALPHABLEND, // ���İ��� ���� ������
    ALPHABLEND_COVERAGE, // ALPHABLEND + AlphaToCoverage
    ONE_ONE, // 1:1 ������
    DECAL, // 0 : AlphaBlend, 1 : One One

    END,
};

// DepthStencilState
enum class DS_TYPE
{
    LESS,
    LESS_EQUAL,

    GREATER, // Decal, Volumesh, �����ִ� ����

    NO_TEST, // �������� X
    NO_WRITE, // �������� Less O, ���� ��� X
    NO_TEST_NO_WRITE, // �������� X, ���̱�� X

    VOLUME_CHECK, // �����޽� ���� üũ�� (RS_TYPE::CULL_NONE)
    STENCIL_CHECK, // Ư�� ���ٽ� ���� ���� �ȼ��� ���

    END,
};


// ���̴� ���� �з�
enum class SHADER_DOMAIN
{
    DOMAIN_DEFERRED, // ����������
    DOMAIN_DECAL, // ��Į
    DOMAIN_LIGHTING, // ���� ó��	

    DOMAIN_OPAQUE, // ������
    DOMAIN_MASK, // ������ or ����
    DOMAIN_TRANSPARENT, // ������ + ����	
    DOMAIN_PARTICLE, // ������ + ����
    DOMAIN_EFFECT, // ������ + ���� + ����
    DOMAIN_POSTPROCESS, // ��ó��

    DOMAIN_NONE, // ������
};

enum class DIR_TYPE
{
    RIGHT,
    UP,
    FRONT,
};

enum ASSET_TYPE
{
    MESH,
    MESH_DATA,
    TEXTURE,
    SOUND,
    PREFAB,
    FLIPBOOK,
    SPRITE,
    MATERIAL,
    GRAPHIC_SHADER,
    COMPUTE_SHADER,
    END,
};

extern const char* ASSET_TYPE_STRING[];


enum class LIGHT_TYPE
{
    DIRECTIONAL, // ���⼺ ����		�¾籤, ��
    POINT, // ������			����, ������, ��ų ����Ʈ, ȶ��
    SPOT, // ����Ʈ ����Ʈ		������, 
};


enum class COMPONENT_TYPE
{
    TRANSFORM, // ��ġ, ũ��, ȸ��, �θ��ڽ� ��������
    COLLIDER2D, // 2D �浹ü
    COLLIDER3D, // 3D �浹ü
    FLIPBOOKPLAYER, // 2D Animation(FlipBook) ���� �� ���
    ANIMATOR3D, // 3D Animation ���� �� ���
    CAMERA, // ī�޶� ���
    LIGHT2D, // 2D ����
    LIGHT3D, // 3D ����
    STATEMACINE, // ���� ����

    MESHRENDER, // �⺻���� ������ ���
    TILEMAP, // 2D Ÿ�ϱ�� ������
    PARTICLE_SYSTEM, // ����(Particle) �ùķ��̼� �� ������
    SKYBOX, // 3D ���
    DECAL, // ���� üũ, ���� ������
    LANDSCAPE, // 3D ���� �ùķ��̼� �� ������

    END,

    SCRIPT, // �뺻
};


// ī�޶� ���� (Projection)
enum PROJ_TYPE
{
    PERSPECTIVE, // ���� ����
    ORTHOGRAPHIC, // ���� ����
};

enum class DEBUG_SHAPE
{
    RECT,
    CIRCLE,
    CROSS,
    LINE,
    CUBE,
    SPHERE,
};


enum SCALAR_PARAM
{
    INT_0,
    INT_1,
    INT_2,
    INT_3,

    FLOAT_0,
    FLOAT_1,
    FLOAT_2,
    FLOAT_3,

    VEC2_0,
    VEC2_1,
    VEC2_2,
    VEC2_3,

    VEC4_0,
    VEC4_1,
    VEC4_2,
    VEC4_3,

    MAT_0,
    MAT_1,
};

enum TEX_PARAM
{
    TEX_0,
    TEX_1,
    TEX_2,
    TEX_3,
    TEX_4,
    TEX_5,

    TEX_ARR_0,
    TEX_ARR_1,
    TEX_ARR_2,
    TEX_ARR_3,

    TEX_CUBE_0,
    TEX_CUBE_1,

    TEX_END,
};


// Task
enum class TASK_TYPE
{
    // 0 : Parent Address, 1 : Child Address
    ADD_CHILD,

    // 0 : Object Address, 1 : Layer Index, 2 : ChildMove
    CREATE_OBJECT,

    // 0 : Object Address
    DELETE_OBJECT,

    // 0 : Level Address
    CHANGE_LEVEL,

    // 0 : NextState
    CHANGE_LEVEL_STATE,

    // 0 : Asset Adress
    DELETE_ASSET,
};


enum class LEVEL_STATE
{
    PLAY,
    PAUSE,
    STOP,
    NONE,
};


enum class PARTICLE_MODULE
{
    SPAWN,
    SPAWN_BURST,
    ADD_VELOCITY,
    SCALE,
    DRAG,
    NOISE_FORCE,
    RENDER,

    END,
};


enum class SCRIPT_PARAM
{
    INT,
    FLOAT,
    VEC2,
    VEC4,

    TEXTURE,
    PREFAB,
};

enum class MRT_TYPE
{
    SWAPCHAIN,
    DEFERRED,
    DECAL,
    LIGHT,

    END,
};
