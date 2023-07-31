#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "CollisionPrimitive.h"
class Collision {
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�~�Ɖ~
	static bool CircleCollision(float X1, float Y1, float R1, float X2, float Y2, float R2);

	static void ConvertToNomalizeVector(XMFLOAT2& out, XMFLOAT2 in);

	static float CalculationVectorLength(const XMFLOAT2& vec01);

	static bool IsCollidingLineAndCircle(Line2D line, Point circle, float dis);

	static float GetLength(XMFLOAT3 position, XMFLOAT3 position2);

	static float GetLength(XMFLOAT2 position, XMFLOAT2 position2);

	//�����蔻��֐�
	//���Ƌ�
	static bool SphereCollision(const XMFLOAT3& pos, const float& R1, const XMFLOAT3& pos2, const float& R2);
	//���Ƌ�2
	static bool CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);
	//���ʂƋ�
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, XMVECTOR* inter = nullptr);
	//�_�ƎO�p�`
	static void ClodestPtPoint2Triangle(const XMVECTOR& point, const Triangle& triangle, XMVECTOR* closest);
	//���ƎO�p�`
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);
	//���C�ƕ���
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane, float* m_Distance = nullptr, XMVECTOR* inter = nullptr);
	//���C�ƎO�p�`
	static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* m_Distance = nullptr, XMVECTOR* inter = nullptr);
	//���C�Ƌ�
	static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* m_Distance = nullptr, XMVECTOR* inter = nullptr);
	//���Ƌ�
	static bool CheckSphere2Box(const Sphere& sphere, const Box& box);
	//OBB��OBB�̓����蔻��
	static bool OBBCollision(OBB& obbA, OBB& obbB);
	// �������ɓ��e���ꂽ���������瓊�e���������Z�o
	static double LenSegOnSeparateAxis(XMVECTOR* Sep, XMVECTOR* e1, XMVECTOR* e2, XMVECTOR* e3 = 0);
};