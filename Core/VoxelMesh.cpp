/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2010 Louise Brown

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

#include "PrecompiledHeaders.h"
#include "VoxelMesh.h"
#include "TexGen.h"
#include "PeriodicBoundaries.h"
#include "ShearedPeriodicBoundaries.h"
#include "StaggeredPeriodicBoundaries.h"
#include <iterator>

#include <locale>
//#define SHINY_PROFILER TRUE

using namespace TexGen;

//Prof Yu
void write_sc(string filename);
void readinp(string filename, vector<string> &inpc);
int checkkeys(string line1, int *skip, int *skip2);
void getcoor(vector<string > &inpc, vector<vector<double> > &coor);//allocate coordinates
void getele(vector<string > &, vector<vector<int> > &);//allocate elements
void get_elset(vector<string > &, vector<string> &, vector<vector<int> > &);//alocate elset
void get_mats(vector<string> &, vector<vector<double> >&, vector<string> &, vector<vector<double> >&);//alocate material
void get_index(vector<string> &, int *, vector<string> &, vector<string> &, vector<vector<int> > &);//Assign elsets index
void get_ori(string, vector<string> &, int);//Read orienatation file
void get_elset2(vector<string> &, vector<int> &);
int find_nth(string, int);
void write_output(string, vector<vector<double> >, vector<vector<int> >, vector<vector<int> >, vector<vector<double> >, int *, vector<string>, vector<vector<double> >);
double volume(vector<vector<double> > &, string);//Calculate sg volume
//X

CVoxelMesh::CVoxelMesh(string Type)
{
	if ( Type == "CShearedPeriodicBoundaries" )
		m_PeriodicBoundaries = new CShearedPeriodicBoundaries;
	else if ( Type == "CStaggeredPeriodicBoundaries" )
		m_PeriodicBoundaries = new CStaggeredPeriodicBoundaries;
	else
		m_PeriodicBoundaries = new CPeriodicBoundaries;
}

CVoxelMesh::~CVoxelMesh(void)
{
	delete m_PeriodicBoundaries;
}

void CVoxelMesh::SaveVoxelMesh( CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, 
							    bool bOutputMatrix, bool bOutputYarns, int iBoundaryConditions, int iElementType )
{
	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
	{
		TGERROR("Unable to create ABAQUS input file: No domain specified");
		return;
	}
	//PROFILE_BEGIN(Begin);
	m_XVoxels = XVoxNum;
	m_YVoxels = YVoxNum;
	m_ZVoxels = ZVoxNum;
	TGLOG("Calculating voxel sizes");
	if ( !CalculateVoxelSizes( Textile ) )
	{
		TGERROR("Unable to create ABAQUS input file: Error calculating voxel sizes");
		return;
	}
	TGLOG("Replacing spaces in filename with underscore for ABAQUS compatibility");
	OutputFilename = ReplaceFilenameSpaces( OutputFilename );
	//GetYarnGridIntersections(Textile);

	//Prof. Yu
	if (OutputFilename.find(".sc") != string::npos) sc_flags = 1;
	else sc_flags = 0;
	//end

	CTimer timer;
	if (sc_flags == 1)
		timer.start("Timing SaveToSwiftComp");
	else
		timer.start("Timing SaveToAbaqus");
	SaveToAbaqus( OutputFilename, Textile, bOutputMatrix, bOutputYarns, iBoundaryConditions, iElementType );
	if (sc_flags == 1) { 
		AddExtensionIfMissing(OutputFilename, ".inp"); 
		string scfilename = OutputFilename.substr(0, OutputFilename.length() - 4);
		write_sc(scfilename);
	}

	if (sc_flags == 1)
		timer.check("End of SaveToSwiftComp");
	else
		timer.check("End of SaveToAbaqus");
	timer.stop();

	string orifile = OutputFilename.substr(0, OutputFilename.length() - 4) + ".ori";
	string eldfile = OutputFilename.substr(0, OutputFilename.length() - 4) + ".eld";
	if (sc_flags == 1){
	  remove(OutputFilename.c_str());
	  remove(orifile.c_str());
	  remove(eldfile.c_str());
	}
	

    //PROFILE_END();
    //PROFILER_UPDATE();
    //PROFILER_OUTPUT("ProfileOutput.txt");
	//SaveToSCIRun( OutputFilename, Textile );
}

/*void CVoxelMesh::SaveShearedVoxelMesh( CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, 
							    bool bOutputMatrix, bool bOutputYarns, int iBoundariesUntiedZ, int iElementType )
{
	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
	{
		TGERROR("Unable to create ABAQUS input file: No domain specified");
		return;
	}
	//PROFILE_BEGIN(Begin);
	m_XVoxels = XVoxNum;
	m_YVoxels = YVoxNum;
	m_ZVoxels = ZVoxNum;
	TGLOG("Calculating voxel sizes");
	
	CalculateShearedVoxelSizes( Textile );
	TGLOG("Replacing spaces in filename with underscore for ABAQUS compatibility");
	OutputFilename = ReplaceFilenameSpaces( OutputFilename );
	//GetYarnGridIntersections(Textile);
	CTimer timer;
	timer.start("Timing SaveToAbaqus");
	SaveToAbaqus( OutputFilename, Textile, bOutputMatrix, bOutputYarns, iBoundariesUntiedZ, iElementType );
	timer.check("End of SaveToAbaqus");
	timer.stop();

    //PROFILE_END();
    //PROFILER_UPDATE();
    //PROFILER_OUTPUT("ProfileOutput.txt");
	//SaveToSCIRun( OutputFilename, Textile );
}*/

// Leaving this in case want to utilise for rendering mesh at some stage (memory allowing)
/*void CVoxelMesh::SaveVoxelMesh(CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, bool bOutputMatrix, bool bOutputYarns )
{
	vector<POINT_INFO> ElementsInfo;
	vector<POINT_INFO>::iterator itElementInfo;
	
	list<int>::iterator itIndex;
	list<int>::iterator itStartIndex;
	int i;
	int iNumNodes = m_Mesh.GetNumNodes(CMesh::HEX);
	int iNumElements;

	m_XVoxels = XVoxNum;
	m_YVoxels = YVoxNum;
	m_ZVoxels = ZVoxNum;
	TGLOG("Calculating voxel sizes");
	CalculateVoxelSizes( Textile );
	TGLOG("Adding nodes");
	
	AddNodes();
	TGLOG("Adding elements");
	AddElements();
	Textile.GetPointInformation(m_Mesh.GetElementCenters(), ElementsInfo);
	list<int> &Indices = m_Mesh.GetIndices(CMesh::HEX);
	TGLOG("Deleting fibre elements");
	iNumElements = ElementsInfo.size();
	if ( !bOutputYarns )
	{
		for ( i = 0, itIndex = Indices.begin(); i < iNumElements && itIndex != Indices.end(); ++i)
		{
			if ( ElementsInfo[i].iYarnIndex == -1 )
			{
				advance( itIndex, iNumNodes );
			}
			else
			{
				itStartIndex = itIndex;
				advance( itIndex, iNumNodes);
				itIndex = Indices.erase( itStartIndex, itIndex );
			}
		}
	}
	else if ( !bOutputMatrix )
	{
		for ( i = 0, itIndex = Indices.begin(), itElementInfo = ElementsInfo.begin(); i < iNumElements && itIndex != Indices.end(); ++i)
		{
			if ( itElementInfo->iYarnIndex == -1 )
			{
				itStartIndex = itIndex;
				advance( itIndex, iNumNodes);
				itIndex = Indices.erase( itStartIndex, itIndex );
				itElementInfo = ElementsInfo.erase( itElementInfo );
			}
			else
			{
				advance( itIndex, iNumNodes );	
				++itElementInfo;
			}
		}
	}
	// else outputting both so don't need to delete anything
	
	TGLOG("Save to ABAQUS");
	//SaveVoxelMeshToVTK( "TestVoxelMesh", ElementsInfo );
	
	m_Mesh.SaveToABAQUS(OutputFilename, bOutputYarns ? &ElementsInfo : NULL, false, false);
	TGLOG("Finished save voxel mesh");
}





void CVoxelMesh::AddNodes()
{
	int x,y,z;
	
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		for ( y = 0; y <= m_YVoxels; ++y )
		{
			for ( x = 0; x <=m_XVoxels; ++x )
			{
				XYZ Point;
				Point.x = m_DomainAABB.first.x + m_VoxSize[0] * x;
				Point.y = m_DomainAABB.first.y + m_VoxSize[1] * y;
				Point.z = m_DomainAABB.first.z + m_VoxSize[2] * z;
				m_Mesh.AddNode( Point );
			}
		}
	}
}

void CVoxelMesh::AddElements()
{
	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int x,y,z;

	for ( z = 0; z < m_ZVoxels; ++z )
	{
		for ( y = 0; y < m_YVoxels; ++y )
		{
			for ( x = 0; x < m_XVoxels; ++x )
			{
				vector<int> Indices;
				Indices.push_back(x + y*numx + z*numx*numy);
				Indices.push_back((x+1) + y*numx + z*numx*numy);
				Indices.push_back((x+1) +y*numx + (z+1)*numx*numy);
				Indices.push_back(x +y*numx + (z+1)*numx*numy);
				Indices.push_back(x + (y+1)*numx + z*numx*numy);
				Indices.push_back((x+1) + (y+1)*numx + z*numx*numy);
				Indices.push_back((x+1) +(y+1)*numx + (z+1)*numx*numy);
				Indices.push_back(x +(y+1)*numx + (z+1)*numx*numy);
				m_Mesh.AddElement(CMesh::HEX, Indices );
			}
		}
	}
}*/

void CVoxelMesh::SaveVoxelMeshToVTK(string Filename, vector<POINT_INFO> &ElementInfo)
{
	CMeshData<int> YarnIndex("YarnIndex", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> YarnTangent("YarnTangent", CMeshDataBase::ELEMENT);
	CMeshData<XY> Location("Location", CMeshDataBase::ELEMENT);
	CMeshData<double> VolumeFraction("VolumeFraction", CMeshDataBase::ELEMENT);
	CMeshData<double> SurfaceDistance("SurfaceDistance", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> Orientation("Orientation", CMeshDataBase::ELEMENT);

	vector<POINT_INFO>::iterator itElementInfo;
	
	for (itElementInfo = ElementInfo.begin(); itElementInfo != ElementInfo.end(); ++itElementInfo)
	{
		YarnIndex.m_Data.push_back(itElementInfo->iYarnIndex);
		YarnTangent.m_Data.push_back(itElementInfo->YarnTangent);
		Location.m_Data.push_back(itElementInfo->Location);
		VolumeFraction.m_Data.push_back(itElementInfo->dVolumeFraction);
		SurfaceDistance.m_Data.push_back(itElementInfo->dSurfaceDistance);
		Orientation.m_Data.push_back(itElementInfo->Orientation);
	}
	

	vector<CMeshDataBase*> MeshData;

	MeshData.push_back(&YarnIndex);
	MeshData.push_back(&YarnTangent);
	MeshData.push_back(&Location);
	MeshData.push_back(&VolumeFraction);
	MeshData.push_back(&SurfaceDistance);
	MeshData.push_back(&Orientation);

	m_Mesh.SaveToVTK(Filename, &MeshData);
}

void CVoxelMesh::SaveToAbaqus( string Filename, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, int iBoundaryConditions, int iElementType )
{  

    AddExtensionIfMissing(Filename, ".inp");

	ofstream Output(Filename.c_str());

	if (!Output)
	{
		TGERROR("Unable to output voxel mesh to ABAQUS file format, could not open file: " << Filename);
		return;
	}

	TGLOG("Saving voxel mesh data to " << Filename);
	
	Output << "*Heading" << endl;
	Output << "File generated by TexGen v" << TEXGEN.GetVersion() << endl;

	Output << "************" << endl;
	Output << "*** MESH ***" << endl;
	Output << "************" << endl;
	Output << "*Node" << endl;
	
		OutputNodes(Output, Textile);
	
	TGLOG("Outputting hex elements");
	//Output the voxel HEX elements
	int iNumHexElements = 0;
	if ( !iElementType )
	{
		Output << "*Element, Type=C3D8R" << endl;
	}
	else
	{
		Output << "*Element, Type=C3D8" << endl;
	}
	iNumHexElements = OutputHexElements( Output, bOutputMatrix, bOutputYarn );
	
	bool bMatrixOnly = false;
	if ( bOutputMatrix && !bOutputYarn )
		bMatrixOnly = true;

	if ( bOutputYarn )
	{
		TGLOG("Outputting orientations & element sets");
		OutputOrientationsAndElementSets( Filename, Output );
	}
	else if ( bMatrixOnly )
	{
		OutputMatrixElementSet( Filename, Output, iNumHexElements, bMatrixOnly );
	}
	
	OutputAllNodesSet( Filename, Output );

	if ( iBoundaryConditions != NO_BOUNDARY_CONDITIONS )
	{
		
		OutputPeriodicBoundaries( Output, Textile, iBoundaryConditions, bMatrixOnly );
	}

	if (sc_flags == 1){
		TGLOG("Finished saving to SwiftComp");
	}
	else
		TGLOG("Finished saving to Abaqus");
		
}

void CVoxelMesh::SaveToSCIRun( string Filename, CTextile &Textile )
{
	Filename = RemoveExtension( Filename, ".inp" );
	Filename += ".hx.pts";

	ofstream NodesFile(Filename.c_str());

	if (!NodesFile)
	{
		TGERROR("Unable to output voxel mesh to SCIRun file format, could not open file: " << Filename);
		return;
	}

	TGLOG("Saving voxel mesh data points to " << Filename);
	
	OutputNodes(NodesFile, Textile, false );
	
	Filename = RemoveExtension( Filename, ".pts" );
	Filename += ".hex";

	ofstream OutputElements(Filename.c_str());

	if (!OutputElements)
	{
		TGERROR("Unable to output voxel mesh to SCIRun file format, could not open file: " << Filename);
		return;
	}

	TGLOG("Outputting hex elements");
	//Output the voxel HEX elements
	int iNumHexElements = 0;
	
	iNumHexElements = OutputHexElements( OutputElements, true, true, false );
	
	TGLOG("Finished saving to SCIRun format");
}





int CVoxelMesh::OutputHexElements(ostream &Output, bool bOutputMatrix, bool bOutputYarn, bool bAbaqus )
{
	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int x,y,z;
	vector<POINT_INFO>::iterator itElementInfo = m_ElementsInfo.begin();
	int iElementNumber = 1;

	vector<POINT_INFO> NewElementInfo;

	if ( !bAbaqus )
		Output << m_XVoxels*m_YVoxels*m_ZVoxels << endl;
	
	for ( z = 0; z < m_ZVoxels; ++z )
	{
		for ( y = 0; y < m_YVoxels; ++y )
		{
			for ( x = 0; x < m_XVoxels; ++x )
			{
				if ( (itElementInfo->iYarnIndex == -1 && bOutputMatrix) 
					 || (itElementInfo->iYarnIndex >=0 && bOutputYarn) )
				{
					if ( bAbaqus )
					{
						Output << iElementNumber << ", ";
						Output << (x+1) +y*numx + z*numx*numy + 1 << ", " << (x+1) + (y+1)*numx + z*numx*numy + 1 << ", ";
						Output << x + (y+1)*numx + z*numx*numy + 1 << ", " << x + y*numx + z*numx*numy + 1 << ", ";
						Output << (x+1) +y*numx + (z+1)*numx*numy + 1 << ", " << (x+1) +(y+1)*numx + (z+1)*numx*numy + 1 << ", ";
						Output << x +(y+1)*numx + (z+1)*numx*numy + 1 << ", " << x +y*numx + (z+1)*numx*numy + 1 << endl;
					}
					else
					{
						Output << x +y*numx + z*numx*numy + 1 << ", " << (x+1) + y*numx + z*numx*numy + 1 << ", ";
						Output << x + y*numx + (z+1)*numx*numy + 1 << ", " << (x+1) + y*numx + (z+1)*numx*numy + 1 << ", ";
						Output << x + (y+1)*numx + z*numx*numy + 1 << ", " << (x+1) +(y+1)*numx + z*numx*numy + 1 << ", ";
						Output << x +(y+1)*numx + (z+1)*numx*numy + 1 << ", " << (x+1) + (y+1)*numx + (z+1)*numx*numy + 1 << endl;
					}
					++iElementNumber;
					if ( bOutputYarn && !bOutputMatrix ) // Just saving yarn so need to make element array with just yarn info
					{
						NewElementInfo.push_back( *itElementInfo );
					}					
					
				}
				++itElementInfo;
			}
		}
	}


	if ( bOutputYarn && !bOutputMatrix )
	{
		m_ElementsInfo.clear();
		m_ElementsInfo = NewElementInfo;
	}
	return ( iElementNumber-1 );
}

void CVoxelMesh::OutputOrientationsAndElementSets( string Filename, ostream &Output )
{
	string OrientationsFilename = Filename;
	OrientationsFilename.replace(OrientationsFilename.end()-4, OrientationsFilename.end(), ".ori");
	ofstream OriOutput(OrientationsFilename.c_str());
	string ElementDataFilename = Filename;
	ElementDataFilename.replace(ElementDataFilename.end()-4, ElementDataFilename.end(), ".eld");
	ofstream DataOutput(ElementDataFilename.c_str());

	if (!OriOutput)
	{
		TGERROR("Unable to output orientations, could not open file: " << OrientationsFilename);
		return;
	}
	if (!DataOutput)
	{
		TGERROR("Unable to output additional element data, could not open file: " << ElementDataFilename);
		return;
	}

	TGLOG("Saving element orientations data to " << OrientationsFilename);
	TGLOG("Saving additional element data to " << ElementDataFilename);

	Output << "********************" << endl;
	Output << "*** ORIENTATIONS ***" << endl;
	Output << "********************" << endl;
	Output << "** Orientation vectors" << endl;
	Output << "** 1st vector represents the fibre direction" << endl;
	Output << "** 2nd vector is an arbitrary vector perpendicular to the first" << endl;
	Output << "*Distribution Table, Name=TexGenOrientationVectors" << endl;
	Output << "COORD3D,COORD3D" << endl;
	Output << "*Distribution, Location=Element, Table=TexGenOrientationVectors, Name=TexGenOrientationVectors, Input=" << StripPath(OrientationsFilename) << endl;
	Output << "*Orientation, Name=TexGenOrientations, Definition=coordinates" << endl;
	Output << "TexGenOrientationVectors" << endl;
	Output << "1, 0" << endl;

	// Default orientation
	OriOutput <<  ", 1.0, 0.0, 0.0,   0.0, 1.0, 0.0" << endl;

	DataOutput << "********************" << endl;
	DataOutput << "*** ELEMENT DATA ***" << endl;
	DataOutput << "********************" << endl;
	DataOutput << "** Element data stored as a depvars " << endl;
	DataOutput << "** 1 - Yarn Index (-1 for matrix, first yarn starting at 0) " << endl;
	DataOutput << "** 2/3 - Location (x and y cross-section coordinates of element relative to yarn centerline) " << endl;
	DataOutput << "** 4 - Volume fraction " << endl;
	DataOutput << "** 5 - Distance of element from the surface of the yarn (for yarn elements only, distance is negative) " << endl;

	int i;
	
	map<int, vector<int> > ElementSets;
	vector<POINT_INFO>::iterator itData;
	for (itData = m_ElementsInfo.begin(), i=1; itData != m_ElementsInfo.end(); ++itData, ++i)
	{
		if (itData->iYarnIndex != -1)
		{
			XYZ Up = itData->Up;
			XYZ Dir = itData->Orientation;
			
			XYZ Perp = CrossProduct(Dir, Up);
			Normalise(Perp);
			OriOutput << i << ", " << Dir << ",   " << Perp << endl;
		}
		else
		{
			// Default orientation
			OriOutput << i << ", 1.0, 0.0, 0.0,   0.0, 1.0, 0.0" << endl;
		}
		DataOutput << i;
		DataOutput << ", " << itData->iYarnIndex;
		DataOutput << ", " << itData->Location;		// This counts as 2 DepVars
		DataOutput << ", " << itData->dVolumeFraction;
		DataOutput << ", " << itData->dSurfaceDistance;
		DataOutput << endl;
		ElementSets[itData->iYarnIndex].push_back(i);
	}

	// Output element sets
	Output << "********************" << endl;
	Output << "*** ELEMENT SETS ***" << endl;
	Output << "********************" << endl;
	Output << "** TexGen generates a number of element sets:" << endl;
	Output << "** All - Contains all elements" << endl;
	Output << "** Matrix - Contains all elements belonging to the matrix" << endl;
	Output << "** YarnX - Where X represents the yarn index" << endl;
	Output << "*ElSet, ElSet=AllElements, Generate" << endl;
	Output << "1, " << m_ElementsInfo.size() << ", 1" << endl;
	vector<int>::iterator itIndices;
	map<int, vector<int> >::iterator itElementSet;
	for (itElementSet = ElementSets.begin(); itElementSet != ElementSets.end(); ++itElementSet)
	{
		if (itElementSet->first == -1)
			Output << "*ElSet, ElSet=Matrix" << endl;
		else
			Output << "*ElSet, ElSet=Yarn" << itElementSet->first << endl;

		WriteValues(Output, itElementSet->second, 16);
	}	
}

void CVoxelMesh::OutputMatrixElementSet( string Filename, ostream &Output, int iNumHexElements, bool bMatrixOnly )
{
	if ( !bMatrixOnly )
		return;

	// Output element sets
	Output << "********************" << endl;
	Output << "*** ELEMENT SETS ***" << endl;
	Output << "********************" << endl;
	Output << "** TexGen generates a number of element sets:" << endl;
	Output << "** All - Contains all elements" << endl;
	Output << "** Matrix - Contains all elements belonging to the matrix" << endl;
	Output << "** YarnX - Where X represents the yarn index" << endl;
	Output << "*ElSet, ElSet=Matrix, Generate" << endl;
	Output << "1, " << iNumHexElements << ", 1" << endl;
}

void CVoxelMesh::OutputAllNodesSet( string Filename, ostream &Output )
{
	Output << "*****************" << endl;
	Output << "*** NODE SETS ***" << endl;
	Output << "*****************" << endl;
	Output << "** AllNodes - Node set containing all elements" << endl;
	Output << "*NSet, NSet=AllNodes, Generate" << endl;
	Output << "1, " << (m_XVoxels+1)*(m_YVoxels+1)*(m_ZVoxels+1) << ", 1" << endl;
}

void CVoxelMesh::OutputPeriodicBoundaries(ostream &Output, CTextile& Textile, int iBoundaryConditions, bool bMatrixOnly )
{
	m_PeriodicBoundaries->SetDomainSize( Textile.GetDomain()->GetMesh() );

	vector<int> GroupA;
	vector<int> GroupB;
	pair< vector<int>, vector<int> > Faces;

	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int numz = m_ZVoxels + 1;
	//int iDummyNodeNum = numx*numy*numz + 1;
	
	// Create a set of nodes for opposite faces of the domain, then output
	for ( int z = 1; z < numz-1; ++z )
	{
		for ( int y = 1; y < numy-1; ++y )
		{
			GroupA.push_back( numx-1 + y*numx + z*numx*numy +1 );
			GroupB.push_back( y*numx + z*numx*numy + 1);
			
		}
	}
	m_PeriodicBoundaries->SetFaceA( GroupA, GroupB );

	GroupA.clear();
	GroupB.clear();
	for ( int z = 1; z < numz-1; ++z )
	{
		for ( int x = 1; x < numx-1; ++x )
		{
			GroupA.push_back( x + (numy-1)*numx + z*numx*numy +1 );
			GroupB.push_back( x + z*numx*numy + 1);
			
		}
	}
	m_PeriodicBoundaries->SetFaceB( GroupA, GroupB );

	GroupA.clear();
	GroupB.clear();
	for ( int y = 1; y < numy-1; ++y )
	{
		for ( int x = 1; x < numx-1; ++x )
		{
			GroupA.push_back( x + y*numx + (numz-1)*numx*numy + 1);
			GroupB.push_back( x + y*numx + 1 );
			
		}
	}
	m_PeriodicBoundaries->SetFaceC( GroupA, GroupB );

	// Create edge node sets
	vector<int> Edges[4];

	for ( int z = 1; z < numz-1; ++z )
	{
		Edges[0].push_back( z*numx*numy + 1 );
		Edges[3].push_back( numx*(numy-1) + z*numx*numy +1 );
		Edges[2].push_back( (z+1)*numx*numy );
		Edges[1].push_back( numx + z*numx*numy );
	}

	for ( int i = 0; i < 4; ++i )
	{
		m_PeriodicBoundaries->SetEdges( Edges[i] );
		Edges[i].clear();
	}
	
	for ( int y = 1; y < numy-1; ++y )
	{
		Edges[3].push_back( y*numx + (numz-1)*numx*numy + 1 ); 
		Edges[2].push_back( (y+1)*numx +(numz-1)*numx*numy );
		Edges[1].push_back( (y+1)*numx );
		Edges[0].push_back( y*numx + 1 );
	}

	for ( int i = 0; i < 4; ++i )
	{
		m_PeriodicBoundaries->SetEdges( Edges[i] );
		Edges[i].clear();
	}

	for ( int x = 1; x < numx-1; ++x )
	{
		Edges[3].push_back( x + (numz-1)*numx*numy + 1 );
		Edges[2].push_back( x + numx*(numy-1) + (numz-1)*numx*numy +1 );
		Edges[1].push_back( x + numx*(numy-1) + 1 );
		Edges[0].push_back( x + 1 );
	}

	for ( int i = 0; i < 4; ++i )
	{
		m_PeriodicBoundaries->SetEdges( Edges[i] );
		Edges[i].clear();
	}

	// Create vertex sets
	// Vertices rearranged to tie up with Li notation (previous numbering in comments)
	m_PeriodicBoundaries->SetVertex( 1 );//5
	m_PeriodicBoundaries->SetVertex( numx );//8
	m_PeriodicBoundaries->SetVertex( numx*numy );//7
	m_PeriodicBoundaries->SetVertex( (numy-1)*numx + 1 );//6
	m_PeriodicBoundaries->SetVertex( (numz-1)*numx*numy + 1 );//1
	m_PeriodicBoundaries->SetVertex( numx + (numz-1)*numx*numy );//4
	m_PeriodicBoundaries->SetVertex( numx*numy*numz );//3
	m_PeriodicBoundaries->SetVertex( numx*(numy-1) + (numz-1)*numx*numy + 1 );//2
	
	m_PeriodicBoundaries->CreatePeriodicBoundaries( Output, numx*numy*numz + 1, Textile, iBoundaryConditions, bMatrixOnly );
}

/// Output data with iMaxPerLine elements per line
template <typename T>
void CVoxelMesh::WriteValues( ostream &Output, T &Values, int iMaxPerLine)
{
	int iLinePos = 0;
	typename T::const_iterator itValue;
	for (itValue = Values.begin(); itValue != Values.end(); ++itValue)
	{
		if (iLinePos == 0)
		{
			// Do nothing...
		}
		else if (iLinePos < iMaxPerLine)
		{
			Output << ", ";
		}
		else
		{
			Output << endl;
			iLinePos = 0;
		}
		Output << *itValue;
		++iLinePos;
	}
	Output << endl;
}

/*void CVoxelMesh::GetYarnGridIntersections( CTextile &Textile )
{
	int x,y,z;
	int iNodeIndex = 1;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;

	vector<pair<XYZ, XYZ> > xyLines;
	vector<pair<XYZ, XYZ> > xzLines;
	vector<pair<XYZ, XYZ> > yzLines;
	
	// Create lines in z direction to test for intersections
	for ( y = 0; y <= m_YVoxels; ++y )
	{
		for ( x = 0; x <=m_XVoxels; ++x )
		{
			XYZ Point1, Point2;
			Point1.x = Point2.x = m_DomainAABB.first.x + m_VoxSize[0] * x;
			Point1.y = Point2.y = m_DomainAABB.first.y + m_VoxSize[1] * y;
			Point1.z = 0.0;
			Point2.z = 1.0;
			xyLines.push_back( make_pair(Point1,Point2) );
		}
	}

	// Create lines in x direction to test for intersections
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		for ( y = 0; y <=m_YVoxels; ++y )
		{
			XYZ Point1, Point2;
			Point1.x = 0.0;
			Point2.x = 1.0;
			Point1.y = Point2.y = m_DomainAABB.first.y + m_VoxSize[1] * y;
			Point1.z = Point2.z = m_DomainAABB.first.z + m_VoxSize[2] * z;
			yzLines.push_back( make_pair(Point1,Point2) );
		}
	}

	// Create lines in y direction to test for intersections
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		for ( x = 0; x <=m_XVoxels; ++x )
		{
			XYZ Point1, Point2;
			Point1.x = Point2.x = m_DomainAABB.first.x + m_VoxSize[0] * x;
			Point1.y = 0.0;
			Point2.y = 1.0;
			Point1.z = Point2.z = m_DomainAABB.first.z + m_VoxSize[2] * z;
			xzLines.push_back( make_pair(Point1,Point2) );
		}
	}
		
	vector<CYarn> &Yarns = Textile.GetYarns();
	vector<CYarn>::iterator itYarn;
	const CDomain* pDomain = Textile.GetDomain();

	//vector< pair<int, pair<double,double>>> xyIntersections;
	vector<vector<pair<int, double> > > xyIntersections;
	vector<vector<pair<int, double> > > xzIntersections;
	vector<vector<pair<int, double> > > yzIntersections;
	xyIntersections.clear();
	xyIntersections.resize(xyLines.size());
	xzIntersections.clear();
	xzIntersections.resize(xzLines.size());
	yzIntersections.clear();
	yzIntersections.resize(yzLines.size());

	int i = 0;
	for ( itYarn = Yarns.begin(); itYarn != Yarns.end(); ++itYarn, ++i)
	{
		CMesh Mesh;
		vector<pair<XYZ, XYZ> >::iterator itLines;

		itYarn->AddSurfaceToMesh(Mesh, *pDomain);
		Mesh.ConvertQuadstoTriangles();

		CalculateIntersections( Mesh, xyLines, i, xyIntersections );
		CalculateIntersections( Mesh, xzLines, i, xzIntersections );
		CalculateIntersections( Mesh, yzLines, i, yzIntersections );
	}
}

void CVoxelMesh::CalculateIntersections( CMesh &Mesh, vector<pair<XYZ,XYZ> > &Lines, int YarnIndex, vector<vector<pair<int, double> > > &Intersections )
{
	vector< pair<double, XYZ> > IntersectionPoints;
	vector< pair<double, XYZ> >::iterator itIntersectionPoints;
	vector<pair<XYZ, XYZ>>::iterator itLines;
	int j = 0;
	double dTol = 1e-8; 

	for( itLines = Lines.begin(); itLines != Lines.end(); ++itLines, ++j )
	{
		int numIntersections;
		IntersectionPoints.clear();
		numIntersections = Mesh.IntersectLine( itLines->first, itLines->second, IntersectionPoints );
		
		pair<int,double>	Intersection;
		Intersection.first = YarnIndex;
		double PrevDist = 0.0;
		for( itIntersectionPoints = IntersectionPoints.begin(); itIntersectionPoints != IntersectionPoints.end(); ++itIntersectionPoints )
		{	
			if ( itIntersectionPoints != IntersectionPoints.begin() && fabs(PrevDist - itIntersectionPoints->first) < dTol )
			{
				continue;
			}
			Intersection.second = itIntersectionPoints->first;
			Intersections[j].push_back(Intersection);
			PrevDist = itIntersectionPoints->first;
		}
	}
}*/



void write_sc(string filename){
    string orifile = filename + ".ori";

	vector<string> inpc;
	vector<vector<double> > coor;
	vector<vector<int> > ele;
	vector<string> elset_name;
	vector<vector<int> > elsets;
	vector<vector<double> > mat;
	vector<vector<double> > expan;
	vector<string> mat_name;
	vector<string> ori;

	readinp(filename, inpc);

	getcoor(inpc, coor);
	getele(inpc, ele);
	get_elset(inpc, elset_name, elsets);
	get_mats(inpc, mat, mat_name, expan);
	int ne = ele.size();
	int *index;
	index = new (nothrow) int[ne];
	//int index[ne];
	get_index(inpc, index, elset_name, mat_name, elsets);
	get_ori(orifile, ori, ne);

	
	write_output(filename, coor, ele, elsets, mat, index, ori, expan);
	//ofstream sc_output;
	//sc_output.open(filename.c_str(),ofstream::app);
	//for (int i = 0; i < inpc.size(); i++)
		//sc_output << inpc[i] << "\n";


	delete[] index;
}

void readinp(string jobname, vector<string> &inpc){
	int i, j, k, jfile, skip = 0, skip2 = 1;

	string line;
	string inpfile = jobname + ".inp";
	ifstream ifile(inpfile.c_str());

	while (getline(ifile, line)){
		i = -1, k = 0;
		/* get rid of blanks */
		do{
			i++;
			if (line[i] == '\0' || line[i] == '\n' || line[i] == '\r') break;
			if (line[i] == ' ' || line[i] == '\t') continue;
			line[k] = line[i];
			k++;
		} while (1);
		line = line.substr(0, k);

		/* check for blank lines and comments */
		if (k == 0) continue;
		if (line[0] == '*'&&line[1] == '*') continue;

		/* changing to uppercase except filenames */
		locale loc;
		j = 0; jfile = 0;
		do{
			if (j >= 6)
			if (strcmp(line.substr(j - 6, 6).c_str(), "INPUT=") == 0) jfile = 1;
			if (j >= 7)
			if (strcmp(line.substr(j - 7, 7).c_str(), "OUTPUT=") == 0) jfile = 1;
			if (j >= 9)
			if (strcmp(line.substr(j - 9, 9).c_str(), "FILENAME=") == 0) jfile = 1;
			if (jfile == 1){
				do{
					if (line[j] != ',') j++;
					else{ jfile = 0; break; }
				} while (j<k);
			}
			else{
				line[j] = toupper(line[j], loc);
			}
			j++;
		} while (j<k);

		/* Skip unnecessary information */
		if (checkkeys(line, &skip, &skip2) == 0) continue;
		if (skip2 == 1) continue;

		inpc.push_back(line);

	}
	ifile.close();

}

int checkkeys(string line1, int *skip, int *skip2){
	int i;
	string keys2[] = { "*NODE", "*ELEMENT", "*ELSET", "*MATERIAL", "*SOLIDSECTION", "*ELASTIC", "*EXPANSION" };
	for (i = *skip; i<7; i++){
		if (line1.find(keys2[i]) != string::npos && i == 0) { *skip = 1; *skip2 = 0; return 1; }
		if (line1.find(keys2[i]) != string::npos && i == 1) { *skip = 2; *skip2 = 0; return 1; }
		if (line1.find(keys2[i]) != string::npos) { *skip2 = 0; return 1; }
		if (line1.substr(0, 1) != "*") return 1;
	}
	if (line1.substr(0, 1) == "*"&&i == 7) { *skip2 = 1; return 0; }
	return 0;
}


void getcoor(vector<string> &inpc, vector<vector<double> > &coor){
	int flags = 0;
	for (unsigned int i = 0; i<inpc.size(); i++){
		if (inpc[i].find("*NODE") != string::npos) { flags = 1; continue; }
		if (inpc[i][0] == '*'&&flags == 1) return;
		if (flags == 1){
			for (unsigned int j = 0; j<inpc[i].length(); j++)
			if (inpc[i][j] == ',') inpc[i][j] = ' ';
			stringstream s(inpc[i]);
			double index, x1, x2, x3;
			if (s >> index >> x1 >> x2 >> x3){
				vector<double> temp;
				temp.push_back(index);
				temp.push_back(x1);
				temp.push_back(x2);
				temp.push_back(x3);
				coor.push_back(temp);
			}
		}
	}
}

void getele(vector<string> &inpc, vector<vector<int> > &ele){
	int flags = 0;
	string eletype;
	for (unsigned int i = 0; i<inpc.size(); i++){
		if (inpc[i].find("*ELEMENT") != string::npos) {
			eletype = inpc[i].substr(14);
			flags = 1; continue;
		}
		if (inpc[i][0] == '*'&&flags == 1) return;
		if (flags == 1){
			for (unsigned int j = 0; j<inpc[i].length(); j++)
			if (inpc[i][j] == ',') inpc[i][j] = ' ';
			stringstream s(inpc[i]);
			int index, e1, e2, e3, e4, e5, e6, e7, e8;
			if (eletype == "C3D8R" || eletype == "C3D8"){
				if (s >> index >> e1 >> e2 >> e3 >> e4 >> e5 >> e6 >> e7 >> e8){
					vector<int> temp;
					temp.push_back(index);
					temp.push_back(e1);
					temp.push_back(e2);
					temp.push_back(e3);
					temp.push_back(e4);
					temp.push_back(e5);
					temp.push_back(e6);
					temp.push_back(e7);
					temp.push_back(e8);
					ele.push_back(temp);
				}
			}
		}
	}
}


void get_elset(vector<string> &inpc, vector<string> &elsetname, vector<vector<int> > &elsets){
	int flags = 0;
	vector<string> temp;
	vector<vector<int> > elset;//Note elset has a blank element at the fr=irst position
	for (unsigned int i = 0; i<inpc.size(); i++){
		if (inpc[i].find("*ELSET") != string::npos){
			elsetname.push_back(inpc[i].substr(13));
			flags = 1;
			vector<int> temp2;
			get_elset2(temp, temp2);
			temp.clear();
			elset.push_back(temp2);
			continue;
		}
		//if(inpc[i][0]=='*'&&flags==1) return;
		if (flags == 1 && inpc[i][0] != '*')
			temp.push_back(inpc[i]);
		else if (inpc[i][0] == '*'&&flags == 1 && inpc[i].substr(0, 6) != "*ELSET") {
			flags = 0;
			vector<int> temp2;
			get_elset2(temp, temp2);
			temp.clear();
			elset.push_back(temp2);
		}
	}
	for (unsigned int i = 1; i<elset.size(); i++)
		elsets.push_back(elset[i]);

}

void get_elset2(vector<string> &temp, vector<int> &temp2){
	for (unsigned int j = 0; j<temp.size(); j++){
		stringstream ss(temp[j]);
		int ii;
		while (ss >> ii){
			temp2.push_back(ii);
			if (ss.peek() == ',')
				ss.ignore();
		}
	}
}

void get_mats(vector<string> &inpc, vector<vector<double> > &mat, vector<string> &mat_name, vector<vector<double> > &expan){
	int flags = 0;
	vector<vector<string> > temp;//record elastic
	vector<string> temp2;//record elastic
	vector<string> exp_tmp;

	for (unsigned int i = 0; i<inpc.size(); i++){
		if (inpc[i].find("*MATERIAL") != string::npos){
			if (flags == 1) { temp.push_back(temp2); temp2.clear(); }
			mat_name.push_back(inpc[i].substr(15));
			continue;
		}
		if (inpc[i].substr(0, 8) == "*ELASTIC") { flags = 1; continue; }

		if (inpc[i].find("*EXPANSION") != string::npos){
			flags = 2;
			temp.push_back(temp2);
			temp2.clear();
			continue;
		}

		if (flags == 2 && inpc[i][0] != '*'){
			exp_tmp.push_back(inpc[i]);
			flags = 0;
		}

		if (flags == 1 && inpc[i][0] != '*') temp2.push_back(inpc[i]);

	}
	//delete comma
	for (unsigned int i = 0; i < temp.size(); i++){
		vector<double> temp3;
		for (unsigned int j = 0; j< temp[i].size(); j++){
			stringstream ss(temp[i][j]);
			double ii;
			while (ss >> ii){
				temp3.push_back(ii);
				if (ss.peek() == ',')
					ss.ignore();
			}
		}
		mat.push_back(temp3);
	}

	for (unsigned int i = 0; i < exp_tmp.size(); i++){
		vector<double> exp_tmp3;
		stringstream ss(exp_tmp[i]);
		double ii;
		while (ss >> ii){
			exp_tmp3.push_back(ii);
			if (ss.peek() == ',')
				ss.ignore();
		}
		expan.push_back(exp_tmp3);
	}
}


void get_index(vector<string> &inpc, int index[], vector<string> &elset_name, vector<string> &mat_name, vector<vector<int> > &elsets){
	vector<string> setname2;
	vector<string> matname2;
	for (unsigned int i = 0; i<inpc.size(); i++){
		if (inpc[i].find("*SOLIDSECTION") != string::npos){
			setname2.push_back(inpc[i].substr(20, find_nth(inpc[i], 2) - 20));
			matname2.push_back(inpc[i].substr(find_nth(inpc[i], 2) + 10, find_nth(inpc[i], 3) - find_nth(inpc[i], 2) - 10));
		}
	}
	for (unsigned int i = 0; i<setname2.size(); i++){
		for (unsigned int j = 0; j<elset_name.size(); j++){
			if (setname2[i] == elset_name[j]){
				for (unsigned int k = 0; k<mat_name.size(); k++)
				if (matname2[i] == mat_name[k]){
					for (unsigned int kk = 0; kk<elsets[j].size(); kk++)
						index[elsets[j][kk] - 1] = k + 1;
				}
			}
		}
	}
}


int find_nth(string line, int nth){
	int count = 0;
	for (unsigned i = 0; i<line.length(); i++){
		if (line[i] == ',') count++;
		if (count == nth || line[i] == '\n' || line[i] == '\0') return i;
	}
	return line.length();
}


void get_ori(string orifile, vector<string> &ori, int ne){
	int count = 0;
	string line;
	ifstream ifile(orifile.c_str());
	if (!ifile) { cout << "Orientation file does not exist" << endl; return; }
	while (getline(ifile, line)){
		count++;
		if (count == 1) continue;
		for (unsigned int i = 0; i<line.length(); i++){
			if (line[i] == ',') line[i] = ' ';
		}
		//line=line.substr(0,line.length()-1);//remove "\n"
		ori.push_back(line);
	}
	if (count != ne + 1) { cout << "Missing elements definition" << endl; return; }
}

void write_output(string jobname, vector<vector<double> > coor1, vector<vector<int> > ele1, vector<vector<int> > elsets1, vector<vector<double> > mat1, int index[], vector<string> ori, vector<vector<double> > expan1){

	int numnodes = coor1.size(), numeles = ele1.size(), nummat = mat1.size();

	ifstream check_analys(jobname.c_str());
	string first_line;
	int type_flags1;
	while (getline(check_analys, first_line)){
		if (first_line.find("#analysis") != string::npos){
			stringstream sss(first_line);
			sss >> type_flags1;
			break;
		}
	}

	


	ofstream out2;
	out2.open(jobname.c_str(),ofstream::app);

	out2 << "3 " << numnodes << " " << numeles << " " << nummat << " 0 0 ";
	out2 << "                #nSG nnode nelem nmate nslave nlayer\n\n";

	for (unsigned int i = 0; i<coor1.size(); i++)
		out2 << coor1[i][0] << " " << coor1[i][1] << " " << coor1[i][2] << " " << coor1[i][3] << "\n";
	out2 << "\n";

	for (unsigned int i = 0; i<ele1.size(); i++){
		out2 << ele1[i][0] << " " << index[i] << " " << ele1[i][1] << " " << ele1[i][2] << " " << ele1[i][3] << " ";
		out2 << ele1[i][4] << " " << ele1[i][5] << " " << ele1[i][6] << " " << ele1[i][7] << " " << ele1[i][8];
		out2 << " 0 0 0 0 0 0 0 0 0 0 0 0\n";
	}
	out2 << "\n";
	for (unsigned int i = 0; i<ori.size(); i++)
		out2 << ori[i] << "  0 0 0\n";

	out2 << "\n\n";
	for (unsigned int i = 0; i<mat1.size(); i++){
		if (mat1[i].size() == 2){
			out2 << i + 1 << " 0 1\n" << "0 0\n";
			out2 << mat1[i][0] << " " << mat1[i][1] << "\n";
			if (type_flags1 == 1)
				out2 << expan1[i][0] << " 0\n";
			out2 << "\n";
		}
		else if (mat1[i].size() == 9){
			out2 << i + 1 << " 1 1\n" << "0 0\n";
			out2 << mat1[i][0] << " " << mat1[i][1] << " " << mat1[i][2] << "\n";
			out2 << mat1[i][6] << " " << mat1[i][7] << " " << mat1[i][8] << "\n";
			out2 << mat1[i][3] << " " << mat1[i][4] << " " << mat1[i][5] << "\n";
			if (type_flags1 == 1)
				out2 << expan1[i][0] << " " << expan1[i][1] << " " << expan1[i][2] << " 0\n";
			out2 << "\n";
		}
	}

	out2 << volume(coor1, jobname) << "\n";

	out2.close();
}

double volume(vector<vector<double> > &coor2, string scfile2){
	ifstream dim_sc(scfile2.c_str());
	string line1st;
	int flags = 0;//check 2D or 3D
	if (dim_sc.good()){
		getline(dim_sc, line1st);
		if (line1st.find("plate") != string::npos)
			flags = 1;
	}
	dim_sc.close();
	double Xmax = 0, Xmin = 0, Ymax = 0, Ymin = 0, Zmax = 0, Zmin = 0;
	//double temp1=0,temp2=0;
	for (int i = 0; i<coor2.size(); i++){
		if (coor2[i][1]>Xmax)
			Xmax = coor2[i][1];
		else if (coor2[i][1]<Xmin)
			Xmin = coor2[i][1];
		if (coor2[i][2]>Ymax)
			Ymax = coor2[i][2];
		else if (coor2[i][2]<Ymin)
			Ymin = coor2[i][2];
		if (coor2[i][3]>Zmax)
			Zmax = coor2[i][3];
		else if (coor2[i][3]<Zmin)
			Zmin = coor2[i][3];
	}
	if (flags == 0) return fabs((Xmax - Xmin)*(Ymax - Ymin)*(Zmax - Zmin));
	else if (flags == 1) return fabs((Xmax - Xmin)*(Ymax - Ymin));
	else return 0;
}
