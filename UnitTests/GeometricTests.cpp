/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#include "GeometricTests.h"
#include "../Core/MatrixUtils.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CGeometricTests);

void CGeometricTests::setUp()
{
}

void CGeometricTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

void CGeometricTests::TestLenticularSection()
{
	double dWidth = 2;
	double dHeight = 1;
	double dDistortion = 0.1;
	CSectionLenticular Section(dWidth, dHeight, dDistortion);
	XY Point;
	Point = Section.GetPoint(0);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5*dWidth, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(dDistortion, Point.y, 1e-9);
	Point = Section.GetPoint(0.25);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5*dHeight, Point.y, 1e-9);
	Point = Section.GetPoint(0.5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5*dWidth, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(dDistortion, Point.y, 1e-9);
	Point = Section.GetPoint(0.75);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5*dHeight, Point.y, 1e-9);
}

void CGeometricTests::TestPointInsideYarn()
{
	// Build a single yarn of diameter 1, going from (0,0,0) to (1,0,0)
	// The domain is 1 x 1 x 1 with center (1,1,1)
	// Build a grid of points inside the domain and check that the analytical solution
	// agrees with the numerical one within a certain tolerance
	CTextile Textile = m_TextileFactory.GetSingleYarn(100, 40);
	CYarn &Yarn = *Textile.GetYarns().begin();
	const CDomain &Domain = *Textile.GetDomain();
	vector<XYZ> Translations = Domain.GetTranslations(Yarn);
	int i, j, k;
	int iGridSize = 10;
	XYZ Point;
	bool bResult;
	pair<XYZ, XYZ> DomainSize = Domain.GetMesh().GetAABB();
	double dDist;
	const double dTolerance = 0.001;
	double dSurfaceDist;
	for (i=0; i<iGridSize; ++i)
	{
		for (j=0; j<iGridSize; ++j)
		{
			for (k=0; k<iGridSize; ++k)
			{
				Point.x = (i+0.5)/iGridSize;
				Point.y = (j+0.5)/iGridSize;
				Point.z = (k+0.5)/iGridSize;

				Point *= DomainSize.second - DomainSize.first;
				Point += DomainSize.first;
				
				bResult = Yarn.PointInsideYarn(Point, Translations, NULL, NULL, NULL, &dSurfaceDist);
				dDist = GetDistanceFromEdge(Point);
				if (bResult)
				{
					CPPUNIT_ASSERT_DOUBLES_EQUAL(dDist, dSurfaceDist, 0.01);
					CPPUNIT_ASSERT(dDist < dTolerance);
				}
				else
				{
					CPPUNIT_ASSERT(dDist > -dTolerance);
				}
			}
		}
	}
}

double CGeometricTests::GetDistanceFromEdge(XYZ Point)
{
	while (Point.y>0.5)
		Point.y -= 1;
	while (Point.z>0.5)
		Point.z -= 1;
	while (Point.y<-0.5)
		Point.y += 1;
	while (Point.z<-0.5)
		Point.z += 1;
	Point.x = 0;
	return GetLength(Point)-0.5;
}

void CGeometricTests::TestQuaternionRotation()
{
	XYZ Vec(1, 0, 0);
	WXYZ Rot(PI/2, 0, 0);
	XYZ Expected(0, 1, 0);
	Vec = Rot * Vec;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Expected.x, Vec.x, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Expected.y, Vec.y, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Expected.z, Vec.z, 1e-6);
}

void CGeometricTests::TestConvertRotation()
{
	// Create a random vector and a random rotation
	XYZ Vec(RandomNumber(-1, 1), RandomNumber(-1, 1), RandomNumber(-1, 1));
	WXYZ RotQuat(RandomNumber(0, 2*PI), RandomNumber(0, 2*PI), RandomNumber(0, 2*PI));
	// Convert the rotation quaternion to a rotation matrix
	CMatrix RotMat = ConvertRotation(RotQuat);
	// Compare the results of rotating by quaternion and matrix
	XYZ VecQuat = RotQuat * Vec;
	XYZ VecMat = RotMat * Vec;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(VecQuat.x, VecMat.x, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(VecQuat.y, VecMat.y, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(VecQuat.z, VecMat.z, 1e-6);
}








