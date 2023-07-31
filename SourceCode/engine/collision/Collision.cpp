#include "Collision.h"

#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;


void Collision::ConvertToNomalizeVector(XMFLOAT2& out, XMFLOAT2 in)
{
	float distance = sqrtf((in.x * in.x) + (in.y * in.y));
	if (distance > 0.0f)
	{
		out.x = in.x / distance;
		out.y = in.y / distance;
	} else
	{
		out = XMFLOAT2(0.0f, 0.0f);
	}
}

float Collision::CalculationVectorLength(const XMFLOAT2& vec01)
{
	return sqrtf((vec01.x * vec01.x) + (vec01.y * vec01.y));
}


bool Collision::IsCollidingLineAndCircle(Line2D line, Point circle, float dis)
{
	// �x�N�g���̍쐬
	auto start_to_center = XMFLOAT2(circle.x - line.start.x, circle.y - line.start.y);
	auto end_to_center = XMFLOAT2(circle.x - line.end.x, circle.y - line.end.y);
	auto start_to_end = XMFLOAT2(line.end.x - line.start.x, line.end.y - line.start.y);
	XMFLOAT2 normal_start_to_end;

	// �P�ʃx�N�g��������
	ConvertToNomalizeVector(normal_start_to_end, start_to_end);

	/*
		�ˉe���������̒���
			�n�_�Ɖ~�̒��S�ŊO�ς��s��
			���n�_ => �I�_�̃x�N�g���͒P�ʉ����Ă���
	*/
	float distance_projection = start_to_center.x * normal_start_to_end.y - normal_start_to_end.x * start_to_center.y;

	// �ˉe�̒��������a����������
	if (fabs(distance_projection) < dis)
	{
		// �n�_ => �I�_�Ǝn�_ => �~�̒��S�̓��ς��v�Z����
		float dot01 = start_to_center.x * start_to_end.x + start_to_center.y * start_to_end.y;
		// �n�_ => �I�_�ƏI�_ => �~�̒��S�̓��ς��v�Z����
		float dot02 = end_to_center.x * start_to_end.x + end_to_center.y * start_to_end.y;

		// ��̓��ς̊|���Z���ʂ�0�ȉ��Ȃ瓖����
		if (dot01 * dot02 <= 0.0f)
		{
			return true;
		}
		/*
			��̏�������R�ꂽ�ꍇ�A�~�͐�����ɂ͂Ȃ��̂ŁA
			�n�_ => �~�̒��S�̒������A�I�_ => �~�̒��S�̒�����
			�~�̔��a�����Z�������瓖����
		*/
		if (CalculationVectorLength(start_to_center) < dis ||
			CalculationVectorLength(end_to_center) < dis)
		{
			return true;
		}
	}

	return false;
}
float Collision::GetLength(XMFLOAT3 position, XMFLOAT3 position2)
{
	float len;
	len = sqrtf(
		(position.x - position2.x) * (position.x - position2.x) + (position.y - position2.y) * (position.y - position2.
			y) + (position.z - position2.z) * (position.z - position2.z));
	return len;
}

float Collision::GetLength(XMFLOAT2 position, XMFLOAT2 position2)
{
	float len;
	len = sqrtf(
		(position.x - position2.x) * (position.x - position2.x) + (position.y - position2.y) * (position.y - position2.
			y));
	return len;
}
bool Collision::SphereCollision(const XMFLOAT3& pos, const float& R1, const XMFLOAT3& pos2, const float& R2) {
	float a = pos.x - pos2.x;
	float b = pos.y - pos2.y;
	float c = pos.z - pos2.z;
	//2�̋������v�Z
	float distance = sqrtf(a * a + b * b + c * c);
	//���a�̍��v���v�Z
	float radius = R1 + R2;
	if (distance <= radius) {
		return true;
	}
	else {
		return false;
	}

	return true;
}

bool Collision::CircleCollision(float X1, float Y1, float R1, float X2, float Y2, float R2) {
	float a = X1 - X2;
	float b = Y1 - Y2;
	//2�̋������v�Z
	float m_Distance = sqrtf(a * a + b * b);
	//���a�̍��v���v�Z
	float radius = R1 + R2;
	if (m_Distance <= radius) {
		return true;
	}
	else {
		return false;
	}
	return true;
}

bool Collision::CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{
	// ���S�_�̋����̂Q�� <= ���a�̘a�̂Q��@�Ȃ����
	float dist = XMVector3LengthSq(sphereA.center - sphereB.center).m128_f32[0];

	float radius2 = sphereA.radius + sphereB.radius;
	radius2 *= radius2;

	if (dist <= radius2) {
		if (inter) {
			// A�̔��a��0�̎����W��B�̒��S�@B�̔��a��0�̎����W��A�̒��S�@�ƂȂ�悤�⊮
			float t = sphereB.radius / (sphereA.radius + sphereB.radius);
			*inter = XMVectorLerp(sphereA.center, sphereB.center, t);
		}
		// �����o���x�N�g�����v�Z
		if (reject) {
			float rejectLen = sphereA.radius + sphereB.radius - sqrtf(dist);
			*reject = XMVector3Normalize(sphereA.center - sphereB.center);
			*reject *= rejectLen;
		}
		return true;
	}

	return false;
}

bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, XMVECTOR* inter)
{
	//���W�n�̌��_���狅�̒��S���W�ւ̋���
	XMVECTOR distV = XMVector3Dot(sphere.center, plane.normal);
	//���ʂ̌��_���������Z���邱�ƂŁ@���ʂƋ��̒��S�Ƃ̋������ł�
	float dist = distV.m128_f32[0] - plane.m_Distance;
	//�����̐�Βl�����a���傫����Γ������ĂȂ�
	if (fabsf(dist) > sphere.radius)
	{
		return false;
	}

	//�^����_���v�Z
	if (inter)
	{
		//���ʏ�̍ŋߐړ_���@�^����_�Ƃ���
		*inter = -dist * plane.normal + sphere.center;
	}
	return true;
}


void Collision::ClodestPtPoint2Triangle(const XMVECTOR& point, const Triangle& triangle, XMVECTOR* closest)
{
	// point��p0�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	XMVECTOR p0_pt = point - triangle.p0;

	XMVECTOR d1 = XMVector3Dot(p0_p1, p0_pt);
	XMVECTOR d2 = XMVector3Dot(p0_p2, p0_pt);

	if (d1.m128_f32[0] <= 0.0f && d2.m128_f32[0] <= 0.0f)
	{
		// p0���ŋߖT
		*closest = triangle.p0;
		return;
	}

	// point��p1�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	XMVECTOR p1_pt = point - triangle.p1;

	XMVECTOR d3 = XMVector3Dot(p0_p1, p1_pt);
	XMVECTOR d4 = XMVector3Dot(p0_p2, p1_pt);

	if (d3.m128_f32[0] >= 0.0f && d4.m128_f32[0] <= d3.m128_f32[0])
	{
		// p1���ŋߖT
		*closest = triangle.p1;
		return;
	}

	// point��p0_p1�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p1��ɑ΂���ˉe��Ԃ�
	float vc = d1.m128_f32[0] * d4.m128_f32[0] - d3.m128_f32[0] * d2.m128_f32[0];
	if (vc <= 0.0f && d1.m128_f32[0] >= 0.0f && d3.m128_f32[0] <= 0.0f)
	{
		float v = d1.m128_f32[0] / (d1.m128_f32[0] - d3.m128_f32[0]);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// point��p2�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	XMVECTOR p2_pt = point - triangle.p2;

	XMVECTOR d5 = XMVector3Dot(p0_p1, p2_pt);
	XMVECTOR d6 = XMVector3Dot(p0_p2, p2_pt);
	if (d6.m128_f32[0] >= 0.0f && d5.m128_f32[0] <= d6.m128_f32[0])
	{
		*closest = triangle.p2;
		return;
	}

	// point��p0_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p2��ɑ΂���ˉe��Ԃ�
	float vb = d5.m128_f32[0] * d2.m128_f32[0] - d1.m128_f32[0] * d6.m128_f32[0];
	if (vb <= 0.0f && d2.m128_f32[0] >= 0.0f && d6.m128_f32[0] <= 0.0f)
	{
		float w = d2.m128_f32[0] / (d2.m128_f32[0] - d6.m128_f32[0]);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// point��p1_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p1_p2��ɑ΂���ˉe��Ԃ�
	float va = d3.m128_f32[0] * d6.m128_f32[0] - d5.m128_f32[0] * d4.m128_f32[0];
	if (va <= 0.0f && (d4.m128_f32[0] - d3.m128_f32[0]) >= 0.0f && (d5.m128_f32[0] - d6.m128_f32[0]) >= 0.0f)
	{
		float w = (d4.m128_f32[0] - d3.m128_f32[0]) / ((d4.m128_f32[0] - d3.m128_f32[0]) + (d5.m128_f32[0] - d6.m128_f32[0]));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{
	XMVECTOR p;
	// ���̒��S�ɑ΂���ŋߐړ_�ł���O�p�`��ɂ���_p��������
	ClodestPtPoint2Triangle(sphere.center, triangle, &p);
	// �_p�Ƌ��̒��S�̍����x�N�g��
	XMVECTOR v = p - sphere.center;
	// �����̓������߂�
	//�i�����x�N�g�����m�̓��ς͎O�����̒藝�̃��[�g�����̎��ƈ�v����j
	float distanceSquare = XMVector3Dot(v, v).m128_f32[0];
	// ���ƎO�p�`�̋��������a�ȉ��Ȃ瓖�����Ă��Ȃ�
	if (distanceSquare > sphere.radius * sphere.radius)	return false;
	// �[����_���v�Z
	if (inter) {
		// �O�p�`��̍ŋߐړ_p���^����_�Ƃ���
		*inter = p;
	}
	// �����o���x�N�g�����v�Z
	if (reject) {
		float ds = XMVector3Dot(sphere.center, triangle.normal).m128_f32[0];
		float dt = XMVector3Dot(triangle.p0, triangle.normal).m128_f32[0];
		float rejectLen = dt - ds + sphere.radius;
		*reject = triangle.normal * rejectLen;
	}
	return true;
}
bool Collision::CheckRay2Plane(const Ray& ray, const Plane& plane, float* m_Distance, XMVECTOR* inter)
{
	const float epsilon = 1.0e-5f;//�덷�z���p�̔����Ȓl

	//�ʖ@���ƃ��C�̕����x�N�g���̓���
	float d1 = XMVector3Dot(plane.normal, ray.dir).m128_f32[0];

	//���ʂɓ�����Ȃ�
	if (d1 > -epsilon) { return false; }

	//�n�_�ƌ��_�Ƃ̋��� (���ʂ̖@������)
	//�ʖ@���ƃ��C�̎n�_���W(�ʒu�x�N�g��)�̓���
	float d2 = XMVector3Dot(plane.normal, ray.start).m128_f32[0];

	//�n�_�ƕ��ʂ̋���(���ʂ̖@������)
	float dist = d2 - plane.m_Distance;

	//�n�_�ƕ��ʂ̋���(���C����)
	float t = dist / -d1;

	//��_���n�_�����ɂ���̂œ�����Ȃ�
	if (t < 0) { return false; }

	//��������������
	if (m_Distance)
	{
		*m_Distance = t;
	}

	//��_���v�Z
	if (inter)
	{
		*inter = ray.start + t * ray.dir;
	}

	return true;
}

bool Collision::CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* m_Distance, XMVECTOR* inter)
{
	//�O�p�`������Ă��镽�ʂ��Z�o
	Plane plane;
	XMVECTOR interPlane;
	plane.normal = triangle.normal;
	plane.m_Distance = XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];
	//���C�ƕ��ʂ��������Ă��Ȃ���Γ������Ă��Ȃ�
	if (!CheckRay2Plane(ray, plane, m_Distance, &interPlane))
	{
		return false;
	}
	//���C�ƕ��ʂ��������Ă����̂ŋ����ƌ�_���������܂ꂽ
	//���C�ƕ��ʂ̌�_���O�p�`�̓����ɂ��邩����
	const float epsilon = 1.0e-5f; //
	XMVECTOR m;
	//��p0_p1�ɂ���
	XMVECTOR pt_p0 = triangle.p0 - interPlane;
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);
	//�ӂ̊O���ł���Γ������Ă��Ȃ��̂Ŕ����ł��؂�
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon)
	{
		return false;
	}


	//��p1_p2�ɂ���
	XMVECTOR pt_1 = triangle.p1 - interPlane;
	XMVECTOR p1_p2 = triangle.p2 - triangle.p1;
	m = XMVector3Cross(pt_1, p1_p2);
	//�ӂ̊O���ł���Γ������Ă��Ȃ��̂Ŕ����ł��؂�
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon)
	{
		return false;
	}

	//��p2_p0�ɂ���
	XMVECTOR pt_2 = triangle.p2 - interPlane;
	XMVECTOR p2_p0 = triangle.p0 - triangle.p2;
	m = XMVector3Cross(pt_2, p2_p0);
	//�ӂ̊O���ł���Γ������Ă��Ȃ��̂Ŕ����ł��؂�
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon)
	{
		return false;
	}

	//�����Ȃ̂œ������Ă���
	if (inter)
	{
		*inter = interPlane;
	}

	return true;
}

bool Collision::CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* m_Distance, XMVECTOR* inter)
{
	XMVECTOR m = ray.start - sphere.center;
	float b = XMVector3Dot(m, ray.dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;
	//
	if (c > 0.0f && b > 0.0f)
	{
		return false;
	}

	float discr = b * b - c;
	//
	if (discr < 0.0f)
	{
		return false;
	}
	//
	//
	float t = -b - sqrtf(discr);
	//
	if (t < 0)
	{
		t = 0.0f;
	}
	if (m_Distance)
	{
		*m_Distance = t;
	}
	if (inter)
	{
		*inter = ray.start + t * ray.dir;
	}

	return true;
}


bool Collision::CheckSphere2Box(const Sphere& sphere, const Box& box)
{
	float sqDistance = 0.0f;
	float pos;

	pos = sphere.center.m128_f32[0];
	if (pos < box.center.m128_f32[0] - box.scale.x)
	{
		sqDistance += (pos - box.center.m128_f32[0] - box.scale.x) * (pos - box.center.m128_f32[0] - box.scale.x);
	}
	else if (pos > box.center.m128_f32[0] + box.scale.x)
	{
		sqDistance += (pos - box.center.m128_f32[0] + box.scale.x) * (pos - box.center.m128_f32[0] + box.scale.x);
	}

	pos = sphere.center.m128_f32[1];
	if (pos < box.center.m128_f32[1] - box.scale.y)
	{
		sqDistance += (pos - box.center.m128_f32[1] - box.scale.y) * (pos - box.center.m128_f32[1] - box.scale.y);
	}
	else if (pos > box.center.m128_f32[1] + box.scale.y)
	{
		sqDistance += (pos - box.center.m128_f32[1] + box.scale.y) * (pos - box.center.m128_f32[1] + box.scale.y);
	}

	pos = sphere.center.m128_f32[2];
	if (pos < box.center.m128_f32[2] - box.scale.z)
	{
		sqDistance += (pos - box.center.m128_f32[2] - box.scale.z) * (pos - box.center.m128_f32[2] - box.scale.z);
	}
	else if (pos > box.center.m128_f32[2] + box.scale.z)
	{
		sqDistance += (pos - box.center.m128_f32[2] + box.scale.z) * (pos - box.center.m128_f32[2] + box.scale.z);
	}

	return sqDistance < sphere.radius* sphere.radius;
}


//OBB�̓����蔻��
bool Collision::OBBCollision(OBB& obbA, OBB& obbB) {
	//���g���Ȃ����
	XMVECTOR not = { 0,0,0 };
	// �e�����x�N�g���̊m��
  // �iN***:�W���������x�N�g���j
	XMVECTOR NAe1 = obbA.GetDirect(0), Ae1 = NAe1 * obbA.GetLen_W(0);
	XMVECTOR NAe2 = obbA.GetDirect(1), Ae2 = NAe2 * obbA.GetLen_W(1);
	XMVECTOR NAe3 = obbA.GetDirect(2), Ae3 = NAe3 * obbA.GetLen_W(2);
	XMVECTOR NBe1 = obbB.GetDirect(0), Be1 = NBe1 * obbB.GetLen_W(0);
	XMVECTOR NBe2 = obbB.GetDirect(1), Be2 = NBe2 * obbB.GetLen_W(1);
	XMVECTOR NBe3 = obbB.GetDirect(2), Be3 = NBe3 * obbB.GetLen_W(2);
	XMVECTOR Interval = obbA.GetPos_W() - obbB.GetPos_W();

	// ������ : Ae1
	double rA = sqrt(
		(Ae1.m128_f32[0] * Ae1.m128_f32[0]) + (Ae1.m128_f32[1] * Ae1.m128_f32[1]) + (Ae1.m128_f32[2] * Ae1.m128_f32[
			2]));
	double rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	double fL = fabs(
		(Interval.m128_f32[0] * NAe1.m128_f32[0]) + (Interval.m128_f32[1] * NAe1.m128_f32[1]) + (Interval.m128_f32[2] *
			NAe1.m128_f32[2]));
	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : Ae2
	rA = sqrt(
		(Ae2.m128_f32[0] * Ae2.m128_f32[0]) + (Ae2.m128_f32[1] * Ae2.m128_f32[1]) + (Ae2.m128_f32[2] * Ae2.m128_f32[
			2]));
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	fL = fabs(
		(Interval.m128_f32[0] * NAe2.m128_f32[0]) + (Interval.m128_f32[1] * NAe2.m128_f32[1]) + (Interval.m128_f32[2] *
			NAe2.m128_f32[2]));

	if (fL > rA + rB)
	{
		return false;
	}
	rA = sqrt(
		(Ae3.m128_f32[0] * Ae3.m128_f32[0]) + (Ae3.m128_f32[1] * Ae3.m128_f32[1]) + (Ae3.m128_f32[2] * Ae3.m128_f32[
			2]));
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	fL = fabs(
		(Interval.m128_f32[0] * NAe3.m128_f32[0]) + (Interval.m128_f32[1] * NAe3.m128_f32[1]) + (Interval.m128_f32[2] *
			NAe3.m128_f32[2]));

	// ������ : Ae3
	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = sqrt(
		(Be1.m128_f32[0] * Be1.m128_f32[0]) + (Be1.m128_f32[1] * Be1.m128_f32[1]) + (Be1.m128_f32[2] * Be1.m128_f32[
			2]));
	fL = fabs(
		(Interval.m128_f32[0] * NBe1.m128_f32[0]) + (Interval.m128_f32[1] * NBe1.m128_f32[1]) + (Interval.m128_f32[2] *
			NBe1.m128_f32[2]));
	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = sqrt(
		(Be2.m128_f32[0] * Be2.m128_f32[0]) + (Be2.m128_f32[1] * Be2.m128_f32[1]) + (Be2.m128_f32[2] * Be2.m128_f32[
			2]));
	fL = fabs(
		(Interval.m128_f32[0] * NBe2.m128_f32[0]) + (Interval.m128_f32[1] * NBe2.m128_f32[1]) + (Interval.m128_f32[2] *
			NBe2.m128_f32[2]));
	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = sqrt(
		(Be3.m128_f32[0] * Be3.m128_f32[0]) + (Be3.m128_f32[1] * Be3.m128_f32[1]) + (Be3.m128_f32[2] * Be3.m128_f32[
			2]));
	fL = fabs(
		(Interval.m128_f32[0] * NBe3.m128_f32[0]) + (Interval.m128_f32[1] * NBe3.m128_f32[1]) + (Interval.m128_f32[2] *
			NBe3.m128_f32[2]));
	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : C11
	XMVECTOR Cross;
	Cross = XMVector3Cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	fL = fabs(
		(Interval.m128_f32[0] * Cross.m128_f32[0]) + (Interval.m128_f32[1] * Cross.m128_f32[1]) + (Interval.m128_f32[2]
			* Cross.m128_f32[2]));
	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : C12
	Cross = XMVector3Cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	fL = fabs(
		(Interval.m128_f32[0] * Cross.m128_f32[0]) + (Interval.m128_f32[1] * Cross.m128_f32[1]) + (Interval.m128_f32[2]
			* Cross.m128_f32[2]));

	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : C13
	Cross = XMVector3Cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	fL = fabs(
		(Interval.m128_f32[0] * Cross.m128_f32[0]) + (Interval.m128_f32[1] * Cross.m128_f32[1]) + (Interval.m128_f32[2]
			* Cross.m128_f32[2]));
	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : C21
	Cross = XMVector3Cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	fL = fabs(
		(Interval.m128_f32[0] * Cross.m128_f32[0]) + (Interval.m128_f32[1] * Cross.m128_f32[1]) + (Interval.m128_f32[2]
			* Cross.m128_f32[2]));

	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : C22
	Cross = XMVector3Cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	fL = fabs(
		(Interval.m128_f32[0] * Cross.m128_f32[0]) + (Interval.m128_f32[1] * Cross.m128_f32[1]) + (Interval.m128_f32[2]
			* Cross.m128_f32[2]));
	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : C23
	Cross = XMVector3Cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	fL = fabs(
		(Interval.m128_f32[0] * Cross.m128_f32[0]) + (Interval.m128_f32[1] * Cross.m128_f32[1]) + (Interval.m128_f32[2]
			* Cross.m128_f32[2]));

	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : C31
	Cross = XMVector3Cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	fL = fabs(
		(Interval.m128_f32[0] * Cross.m128_f32[0]) + (Interval.m128_f32[1] * Cross.m128_f32[1]) + (Interval.m128_f32[2]
			* Cross.m128_f32[2]));

	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : C32
	Cross = XMVector3Cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	//L = XMVector3Dot(Interval, Cross);
	fL = fabs(
		(Interval.m128_f32[0] * Cross.m128_f32[0]) + (Interval.m128_f32[1] * Cross.m128_f32[1]) + (Interval.m128_f32[2]
			* Cross.m128_f32[2]));

	if (fL > rA + rB)
	{
		return false;
	}
	// ������ : C33
	Cross = XMVector3Cross(NAe3, NBe3);

	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	//L = XMVector3Dot(Interval, Cross);
	fL = fabs(
		(Interval.m128_f32[0] * Cross.m128_f32[0]) + (Interval.m128_f32[1] * Cross.m128_f32[1]) + (Interval.m128_f32[2]
			* Cross.m128_f32[2]));

	if (fL > rA + rB)
	{
		return false;
	}

	// �������ʂ����݂��Ȃ��̂Łu�Փ˂��Ă���v
	return true;
}

// �������ɓ��e���ꂽ���������瓊�e���������Z�o
double Collision::LenSegOnSeparateAxis(XMVECTOR* Sep, XMVECTOR* e1, XMVECTOR* e2, XMVECTOR* e3)
{
	// 3�̓��ς̐�Βl�̘a�œ��e���������v�Z
	// ������Sep�͕W��������Ă��邱��
	double fr1 = fabs(
		(Sep->m128_f32[0] * e1->m128_f32[0]) + (Sep->m128_f32[1] * e1->m128_f32[1]) + (Sep->m128_f32[2] * e1->m128_f32[
			2]));
	double fr2 = fabs(
		(Sep->m128_f32[0] * e2->m128_f32[0]) + (Sep->m128_f32[1] * e2->m128_f32[1]) + (Sep->m128_f32[2] * e2->m128_f32[
			2]));
	double fr3 = e3
		? (fabs(
			(Sep->m128_f32[0] * e3->m128_f32[0]) + (Sep->m128_f32[1] * e3->m128_f32[1]) + (Sep->m128_f32[2]
				* e3->m128_f32[2])))
		: 0;
	return fr1 + fr2 + fr3;
}


