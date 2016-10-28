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

#pragma once
#include "Interpolation.h"

namespace TexGen
{
	using namespace std;

	/// Bezier interpolation for yarn paths
	class CLASS_DECLSPEC CInterpolationAdjusted : public CInterpolation
	{
	public:
		CInterpolationAdjusted(const CInterpolation &Interpolation);
		CInterpolationAdjusted(TiXmlElement &Element);
		~CInterpolationAdjusted(void);

		CInterpolation* Copy() const { return new CInterpolationAdjusted(*this); }
		string GetType() const { return "CInterpolationAdjusted"; }
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// Calculate the node tangents (use node tangents of they exist) and store them in m_Tangents
		void Initialise(const vector<CNode> &MasterNodes) const;

		/// Get a node from parametric function where t is specified
		CSlaveNode GetNode(const vector<CNode> &MasterNodes, int iIndex, double t) const;

		/// At given index and value t the position of the node should be adjusted by given vector
		void AddAdjustment(int iIndex, double t, XYZ Vector);

	protected:
		/// Represents the adjustements to the interpolation
		/**
		The first value in the pair represents the T value (distance along the yarn), the second
		value represents the displacement vector.
		*/
		vector<vector<pair<double, XYZ> > > m_Adjustments;
		CObjectContainer<CInterpolation> m_pInterpolation;
	};

};	// namespace TexGen







