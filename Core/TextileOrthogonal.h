/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2011 Louise Brown
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
#include "Textile3DWeave.h"
#include "SectionPowerEllipse.h"
#include "SectionEllipse.h"
#include "SectionHybrid.h"

namespace TexGen
{ 
	using namespace std;

	
	/// Represents a 3D orthogonal woven textile
	class CLASS_DECLSPEC CTextileOrthogonal : public CTextile3DWeave
	{
	public:

		/// Build a weave unit cell of given width, height, number of layers, yarn spacing and fabric thickness
		CTextileOrthogonal(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing,
							double dXHeight, double dYHeight, bool bRefine = false, bool bWeavePattern = false );
		CTextileOrthogonal(TiXmlElement &Element);
		virtual ~CTextileOrthogonal(void);

		virtual CTextile* Copy() const { return new CTextileOrthogonal(*this); }
		virtual string GetType() const { return "CTextileOrthogonal"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		void SwapBinderPosition( int x, int y );

		/// Find min and max z values for the textile
		//virtual void FindMinMaxZ( double& dMinZ, double& dMaxZ );
		virtual void SetupLayers( int iNumWarpLayers, int iNumWeftLayers, int iNumBinderLayers = 1 );
		/// Build the textile. 
		/** Includes calls to functions to refine the textile to achieve a given thickness 
		if this option has been selected.
		*/
		virtual bool BuildTextile() const;

		virtual void ConvertToPatternDraft( /*CPatternDraft& PatternDraft*/ );
		/// Set up row of pattern cells for one weft pattern using one row of weave pattern data and the layers pattern for the weave
		virtual void SetupWeftRow( vector<int>& Layers, vector<int>& Row, int NumWarps, int Weft );
		void ConsolidateCells();

	protected:
		/// Check if binder yarn widths are greater than through thickness spacing. Adjust to fit if necessary
		void CheckBinderWidths() const;
		/// Shape the binder yarns around the weft at top and bottom
		void ShapeBinderYarns() const;
		/// Add extra nodes to binder yarns to match shape of adjacent weft yarns
		int AddBinderNodes( int CurrentNode, int i, int j ) const;
		/// Adjust binder yarn cross sections to fit specified yarn thickness
		void AdjustBinderYarns() const;
		/// Adjust binder section for specified i,j position
		void AdjustBinderYarnSection( int i, int j ) const;
		void AdjustBinderYarnSection( int i, int j, int BinderYarnIndex, int WeftYarnIndex, int OppositeWeftYarnIndex, bool IsTop ) const;
		/// Adjust height of inside layers to meet specified textile thickness, keeping yarn cross-sectional areas constant
		bool AdjustMidLayerHeights() const;

		void AdjustOuterWeftYarns() const;

		void ChangeWeftSection( CSection& TopHalf, CSection&BottomHalf, int YarnIndex, int Node, double Offset) const;
		void AdjustWeftToFitBinderHeight( int i, int j, CSectionPowerEllipse& PowerEllipseSection, double& MaxWidth, double& TargetArea, double& MaxVolumeFraction, double& FibreArea ) const;
		bool AdjustWeftHeight( CSectionPowerEllipse& PowerEllipseSection, double& MaxWidth, double& TargetArea, double& MaxVolumeFraction, double& FibreArea, double TargetHeight ) const;
		void AdjustBinderPosition( int iCellIndex, int i, int j, double dHeight, bool bIsTop ) const;
		void ChangeBinderSection( CSection& TopHalf, CSection&BottomHalf, int YarnIndex, int Node, double Offset) const;
		/// Offset yarn at position i,j at cell height k by dHeight in the z direction
		void OffsetYarn( int i, int j, int k, double dHeight, int YarnType ) const;

		/// Get height of hybrid section.  Assumes max height at mid-point
		double GetHybridHeight( CSectionHybrid* Section ) const;
		/// Get width of hybrid section
		double GetHybridWidth( CSectionHybrid* Section ) const;
		/// Replace the yarn cross section at the given index
		void ReplaceYarnSection( int YarnIndex, int Node, CSection& Section ) const;
		/// Replace binder cross section with default if at top, adjusted if through thickness with spacing narrower than yarn width
		void ReplaceThroughBinderSection( int BinderYarnIndex, int Node, CSection& Section, map<pair<int, int>, YARNDATA>::iterator& itBinderData ) const;

		void ChangePowerEllipseSection( double& TargetArea, CSectionPowerEllipse* YarnSection, double FibreArea ) const;

		int FindNextBinderIndex( int StartIndex ) const;

		/// Find the height of the binder yarn within a cell
		int FindBinderHeight( const vector<PATTERN3D>& Cell ) const;

		/// Move binder yarn to top/bottom of weft stack if there are no_yarns in cell
		void MoveBinderYarnPosition( vector<PATTERN3D> &Cell );

		/// Get section point for power ellipse or hybrid section
		XY GetSectionPoint( CSection* YarnSection, double t ) const;

		/// Correct interference int warp yarn caused by creating crimp in top/bottom weft yarns
		void CorrectCrimpInterference( int i, int j, int WeftYarnIndex, bool IsTop) const;
		///
		double GetHybridSectionWidth( int WarpIndex, int WeftIndex ) const;

		int GetWeavePatternYarnIndex(int x, int y, int z) const;

		vector<int> &GetYarnCell(int x, int y);
		const vector<int> &GetYarnCell(int x, int y) const;
		void RemoveYarnCell(int x, int y);
		bool BuildWeavePatternTextile() const;
		mutable int m_iYYarnOffset;

		mutable map<pair<int, int>, YARNDATA> m_BinderData;
		bool m_bWeavePattern;
		vector<vector<int> > m_WeftYarns;
	};
};	// namespace TexGen
