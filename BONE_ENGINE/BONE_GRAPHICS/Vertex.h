#pragma once
#include "Common.h"

struct VERTEX
{
	Vector3 p;
	Vector3 n;
	Vector2 uv;
	static const int FVF;

	VERTEX() {}
	VERTEX(float x, float y, float z, float _nX, float _nY, float _nZ, float _fU, float _fV)
	{
		p.x = x;
		p.y = y;
		p.z = z;

		n.x = _nX;
		n.y = _nY;
		n.z = _nZ;

		uv.x = _fU;
		uv.y = _fV;
	}

	VERTEX(float x, float y, float z)
	{
		p.x = x;
		p.y = y;
		p.z = z;
	}

	VERTEX(float x, float y, float z, float _fU, float _fV)
	{
		p.x = x;
		p.y = y;
		p.z = z;

		uv.x = _fU;
		uv.y = _fV;
	}
};

struct VERTEX_INDEX {
	WORD _0, _1, _2;
};
void SetVtx3DTexture(VERTEX &ct, Vector3 p, Vector3 n, float tu, float tv);
void SetIdx(VERTEX_INDEX &ci, WORD _0, WORD _1, WORD _2);
float CalculateDistance(Vector3 a, Vector3 b);