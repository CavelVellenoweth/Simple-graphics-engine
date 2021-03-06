//--------------------------------------------------------------------------------------------
// File:		MyCode.cpp
// Version:		V2.0
// Author:		Daniel Rhodes
// Description:	Your code goes here
// Notes:		For use with OpenGl 2.0 / DirectX9.0c or higher
//--------------------------------------------------------------------------------------------

// System header files
#include <math.h>

// Windows header files
// --

// Custom header files
#include "MyCode.h"
#include "resource.h"
extern HINSTANCE g_hInstance;
// For dynamic keyboard control
	int m_iMoveX;
	int m_iMoveY;
	int m_iMoveZ;	
	float m_dThetaX;
	float m_dThetaY;
	float m_dThetaZ;
	double m_dViewAngle;
	//debugging control variables
	int iFaceSelection;			// FaceSelection > 0: enables one face (with index selecton - 1)
								// to be drawn on its own to aid debugging.
	bool bDebug;				// Switches on debug output to file when true.
	// number of polygons read in
	int m_iNumOfPolys;
	//Frame Count
	int m_iOurFrameCount;
	// Input filename
	char m_sFilename[30];
	// Viewpoint data, use for parts B/C
	VECTOR m_vDisp, m_vLight;	// Displacement of object coordinates relative
								// to viewing coordinates and light direction
	//Debug File Handle
	FILE *debugfile;



	// Drawing area dimensions: width and height
	int m_iWidth;
	int m_iHeight;

	//Drawing Surface Handle
	float *m_fDrawingSurface;
// Database data, for the polygons
POLYGON polylist[2000];  // Array to store a list of polygons.

COLOUR Default_Colour={0.5,0.5,0.5};//colours are floats these days
//-----------------------------------------------------------------------------
// Name: KeyboardControl
// Desc: Enable Keyboard Control
//-----------------------------------------------------------------------------
void KeyboardControl( WPARAM wParam )
{
	switch( wParam ) 
	{
	case VK_ESCAPE:	// Exit application
		PostQuitMessage( 0 );
		break;
	case VK_LEFT:
		m_iMoveX--;
		break;
	case VK_RIGHT:
		m_iMoveX++;
		break;
	case VK_UP:
		m_iMoveY--;
		break;
	case VK_DOWN:
		m_iMoveY++;
		break;
	case VK_ADD:
		m_iMoveZ++;
		break;
	case VK_SUBTRACT:
		m_iMoveZ--;
		break;
	case VK_D:
		bDebug = !bDebug; // Toggle Debug output
		break;
	case VK_Q: 	DialogBoxParam( g_hInstance, MAKEINTRESOURCE(IDD_INIT),run->m_hWindow, run->DialogMessageHandlerStatic, (LPARAM)(run));
		break;

	}
}

//-----------------------------------------------------------------------------
// Name: KeyboardControl
// Desc: Enable Keyboard Control
//-----------------------------------------------------------------------------
void MouseControl( POINT currentMousePos, POINT oldMousePos )
{
	m_dThetaX -= ( currentMousePos.x - oldMousePos.x );
	m_dThetaY -= ( currentMousePos.y - oldMousePos.y );
}


//-----------------------------------------------------------------------------
// Name: displayFinal
// Desc: Routine to display useful info after program exit
//-----------------------------------------------------------------------------
void displayFinal( )
{
	char sDispString[50];
	sprintf( sDispString, "Total Framecount %d", m_iOurFrameCount );
	run->Alert( "Finished", sDispString );
}

//-----------------------------------------------------------------------------
// Name: displayReadInfo
// Desc: Routine to display useful info after file read, shows light vector
//		 as an example, modify and use for debugging as required
//-----------------------------------------------------------------------------
void displayReadInfo( )
{
	char sDispString[50];
	sprintf( sDispString, "%d polygons read", m_iNumOfPolys );
	run->Alert( m_sFilename, sDispString );
	sprintf( sDispString , "Light Vector %f  %f  %f", m_vLight.x, m_vLight.y, m_vLight.z );
	run->Alert( "Start Values:", sDispString );
}


//-----------------------------------------------------------------------------
// Name: LoadPolys
// Desc: Read polygon info from file
//-----------------------------------------------------------------------------
int LoadPolys( FILE *infile )
{
	char cInString[1000];
	int iNumPolys = 0;
	float fLength;
	float fR, fG, fB;		// red, green, blue values

	do
	{
		fgets( cInString, 1000, infile);						 // Read first/next line of file
		sscanf( cInString, "%d", &polylist[iNumPolys].nv);   // Get number of vertices
		fprintf( debugfile, "number of vertices: %d\n", polylist[iNumPolys].nv);   // print number of vertices to debug file
		if  (polylist[iNumPolys].nv == 0)
			break;	// Get out if terminating zero found

		// Only allocate the memory we need - ALWAYS remember to delete on shutdown

		for (int i = 0; i < polylist[iNumPolys].nv; i++)
		{
			// Read next line of file
			fgets(cInString, 1000, infile);
			//Get Coordinates
			sscanf(cInString, "%f%f%f", &( polylist[iNumPolys].vert[i].x ),
										&( polylist[iNumPolys].vert[i].y ),
										&( polylist[iNumPolys].vert[i].z ) );
		}
	
		polylist[iNumPolys].normal = Cross( VectorDiff( polylist[iNumPolys].vert[0], polylist[iNumPolys].vert[1] ),
									 VectorDiff( polylist[iNumPolys].vert[0],polylist[iNumPolys].vert[2] ) );
		fLength = (float)sqrt( Dot( polylist[iNumPolys].normal, polylist[iNumPolys].normal ) ); // Calculate length of vector

	    polylist[iNumPolys].normal.x /= fLength;	// Normalise
	    polylist[iNumPolys].normal.y /= fLength;	// each
	    polylist[iNumPolys].normal.z /= fLength;	// component
	    fgets(cInString, 1000, infile);		// Read  next line of file
	    sscanf( cInString, "%f%f%f", &fR, &fG, &fB );	// Get Colour, texture
		if (fR>1.0 ||fG>1.0||fB>1.0)//cope with either Open Gll 0-1.0 colours or old style 0-255 colours
		{
			polylist[iNumPolys].colour.r = fR/255.0;
			polylist[iNumPolys].colour.g = fG/255.0;
			polylist[iNumPolys].colour.b = fB/255.0;	
		}
		else
		{
			polylist[iNumPolys].colour.r = fR;
			polylist[iNumPolys].colour.g = fG;
			polylist[iNumPolys].colour.b = fB;	
		}
		iNumPolys++;
	} while( 1 );

	return iNumPolys;  //Return number of polygons read
}


//-----------------------------------------------------------------------------
// Name: ReadFile
// Desc: Read polygon info from file
//-----------------------------------------------------------------------------
void ReadFile()
{
	FILE *flInFile;
	flInFile = fopen( m_sFilename, "r" );
	m_iNumOfPolys = LoadPolys( flInFile );
	displayReadInfo();
	fclose(flInFile);	
}


//-----------------------------------------------------------------------------
// Name: Plotpix
// Desc: Draw a pixel - Calls nRGBAImage::SetColour(...),
//		 m_kImage MUST be initialised before use!
//		 Example usage: Plotpix( x, y, 255, 0, 0 );
//-----------------------------------------------------------------------------
inline void Plotpix( DWORD dwX, DWORD dwY, float fR, float fG, float fB )
{
	DWORD dwYtemp;

	// If using OpenGL we need to mirror the Y coordinates,
	// as OpenGL uses the opposite coordinate system to us and DirectX
#ifdef 	DIRECTX 
	dwYtemp = dwY;
#else  //OPENGL should be defined
	dwYtemp = ( m_iHeight - 1 ) - dwY;
#endif

	int index = 4 * ( dwYtemp * m_iWidth + dwX );
	m_fDrawingSurface[index]	 = fR;	// Red Channel
	m_fDrawingSurface[index + 1] = fG;	// Green Channel
	m_fDrawingSurface[index + 2] = fB;	// Blue Channel
	m_fDrawingSurface[index + 3] = 0.0; // Alpha Channel
}

//-----------------------------------------------------------------------------
// Name: DrawImage
// Desc: Draws the image
//-----------------------------------------------------------------------------
void DrawImage( )
{
	POLYGON polyTempP, polyTempQ, polyTempQT;	// Temporary polygons for copying transformed, projected / clipped
												// versions of scene polys before drawing them. 
	int iCurrentPoly;							// Current polygon in process
	
	if ( m_iNumOfPolys <= 0 )
		DrawSquare(Default_Colour );		// This draws the square you see, replace with Trapezium, and later Polygon
	else
		m_iOurFrameCount++;	// Increment frame counter if we have a polygon to draw

	for ( iCurrentPoly = 0; iCurrentPoly < m_iNumOfPolys; iCurrentPoly++ )	// for each polygon
	{
		if ( iFaceSelection > m_iNumOfPolys )
			iFaceSelection = m_iNumOfPolys;    //Keep debug selector in range
		
		
		if ( iFaceSelection && ( iCurrentPoly + 1 ) != iFaceSelection)
			continue; // Reject unselected polygons if debug selection active.
		
	
		polyTempP = polylist[iCurrentPoly];             //copy static data into temp poly structure
		
		// Copy each vertex in polygon, add displacement to allow shift
		for (int i = 0; i < polylist[iCurrentPoly].nv; i++)
		{
			polyTempP.vert[i].x = polylist[iCurrentPoly].vert[i].x + m_iMoveX;
			polyTempP.vert[i].y = polylist[iCurrentPoly].vert[i].y + m_iMoveY;
		}
				
		if ( bDebug )
			fprintf( debugfile, " number of vertices: %d\n", polyTempP.nv);   // print number of vertices

		fflush( debugfile );
		
		if ( bDebug )	// Print out current poly specs if debug active
		{
			for (int i = 0; i < polyTempP.nv; i++)
			{
				fprintf( debugfile, "before clipping Polygon %d, Vertex %d values: %7.2f, %7.2f, %11.6f\n",
						 iCurrentPoly, i, polyTempP.vert[i].x, polyTempP.vert[i].y, polyTempP.vert[i].z ); 
			}
			fflush( debugfile );
			
		}
		// The section below calls clipping and polygon draw routines, commented out to allow the 
		// program to work without them. You may re-instate once you have appropriate routines,
		// or replace with your own code.
		/*
		ClipPolyXHigh( &polyTempP, &polyTempQT, WINDOWWIDTH );	// Clip against upper x boundary
		ClipPolyYHigh( &polyTempQT, &polyTempQ, WINDOWHEIGHT );	// Clip against upper y boundary (bottom of screen)
		ClipPolyXLow( &polyTempQ, &polyTempQT, 0);				// Clip against lower x boundary
		ClipPolyYLow( &polyTempQT, &polyTempQ, 0);				// Clip against lower y boundary (bottom of screen)
		
		if ( bDebug )	// Print out current poly specs if debug active
		{
			for ( int i = 0; i < polyTempQ.nv; i++ )
				fprintf( debugfile, "after clipping Polygon %d Vertex %d values:y %7.2f  %7.2f %11.6f\n",
						 iCurrentPoly, i, polyTempQ.vert[i].x, polyTempQ.vert[i].y, polyTempQ.vert[i].z ); 

			fflush(debugfile);
		}
		  
		DrawPolygon( &polyTempQ );	// Call the drawing routine
		*/
	}
	if ( m_iNumOfPolys > 0 )
		bDebug = false;	// Switch debug off after first run - switch on again via keyboard control if needed

	
}

//-----------------------------------------------------------------------------
// Name: DrawSquare
// Desc: Draw a sqaure
//-----------------------------------------------------------------------------
void DrawSquare(COLOUR c )
{
	//Note no protection to keep in screen bounds...
	for ( int i = m_iMoveX; i < 50 + m_iMoveX; i++)
	{
		for( int j = m_iMoveY; j < 50 + m_iMoveY; j++)
		{
			Plotpix( i, j, c.r, c.g, c.b );
		}
	}	
}

/*
//-----------------------------------------------------------------------------
// Name: DrawPolygon
// Desc: Draw a polygon
//-----------------------------------------------------------------------------
void DrawPolygon( ... )
{
	// Your code here
}
*/

//-----------------------------------------------------------------------------
// Name: ClipPolyXLow
// Desc: Clipping Routine for lower x boundary
//-----------------------------------------------------------------------------
int ClipPolyXLow(POLYGON *pIinput, POLYGON *pOutput, int iXBound)
{
	// Tell calling routine how many vertices in pOutput array
	return pOutput->nv;
}

//-----------------------------------------------------------------------------
// Name: ClipPolyYLow
// Desc: Clipping Routine for lower y boundary
//-----------------------------------------------------------------------------
int ClipPolyYLow(POLYGON *pIinput, POLYGON *pOutput, int iYBound)
{
	// Tell calling routine how many vertices in pOutput array
	return pOutput->nv;
}

//-----------------------------------------------------------------------------
// Name: ClipPolyXHi
// Desc: Clipping Routine for upper x boundary
//-----------------------------------------------------------------------------
int ClipPolyXHigh(POLYGON *pIinput, POLYGON *pOutput, int iXBound)
{
	// Tell calling routine how many vertices in pOutput array
	return pOutput->nv;
}

//-----------------------------------------------------------------------------
// Name: ClipPolyYHi
// Desc: Clipping Routine for upper y boundary
//-----------------------------------------------------------------------------
int ClipPolyYHigh(POLYGON *pIinput, POLYGON *pOutput, int iYBound)
{
	// Tell calling routine how many vertices in pOutput array
	return pOutput->nv;
}

//-----------------------------------------------------------------------------
// Name: Init
// Desc: Initialises Direct3D etc.
//		 This is called before switch to graphics mode,
//		 example of z buffer initialisation shown in comments,
//		 ignore for parts a/b.
//-----------------------------------------------------------------------------
