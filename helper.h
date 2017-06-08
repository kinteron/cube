#pragma once
#include "logic.h"
#define PI 3.141592f


//gxm-data
static const SceGxmProgramParameter *s_wvpParam = NULL;
static const SceGxmProgramParameter *s_rotationParam = NULL;

/*	Data structure for clear geometry */
typedef struct ClearVertex
{
	float x;
	float y;
} ClearVertex;

//used in main
//!! The program parameter for the transformation of the triangle
static Matrix4 s_finalTransformation;

//value von Rotation
static Matrix4 s_rotationMatrix;

//output text
static char buf[32];
static char text[512];

//prototypes
int isButtonPressed(int button);	// input any button
void logicBuild();	// build cube in cube.cpp

/*	Data structure for basic geometry */
typedef struct BasicVertex
{
	float position[3];  // Easier to index.
	uint32_t color; // Data gets expanded to float 4 in vertex shader.
	uint8_t flag;	//rotationFlag between 0 - 1
} BasicVertex;

//gxm data
// !! Data added.
static SceGxmVertexProgram		*s_basicVertexProgram = NULL;
static SceGxmFragmentProgram	*s_basicFragmentProgram = NULL;
static BasicVertex				*s_basicVertices = NULL;
static uint16_t					*s_basicIndices = NULL;
static int32_t					s_basicVerticesUId;
static int32_t					s_basicIndiceUId;

// collect horizontal slices
static BasicVertex s_slice_horizontal_1[12];
static BasicVertex s_slice_horizontal_2[12];
static BasicVertex s_slice_horizontal_3[12];

// collect vertical slices
static BasicVertex s_slice_vertical_1[12];
static BasicVertex s_slice_vertical_2[12];
static BasicVertex s_slice_vertical_3[12];

//input variables (key and touch)
static bool isPressed = false;

void clearRotFlag(BasicVertex cube[]);	// stop rotation for vertices
void savePointsIntoSliceArray(int index, BasicVertex *field);
void drawPointsAndCreateCube();

static Logic logic;

//logic relevant
static Edge white_edges[3], orange_edges[3], yellow_edges[3], blue_edges[3], green_edges[3], red_edges[3];

float toRadians(float degrees){
	return degrees * (PI/180);
}

//external logic
void logicBuild() {

	logic.initFaces();
	referenceBlock.size = .4f;
	referenceBlock.color = 0x00000000;
	referenceBlock.offset = .1f;

	white_edges[4];	//array
	orange_edges[4]; //array
	yellow_edges[4]; //array
	blue_edges[4]; //array
	green_edges[4];	//array
	red_edges[4]; //array

	logic.createEdgesAsVectors_InWorldSpace(white_edges, &logic.white_F);	//top,left,bottom,right are saved into edges
	logic.createEdgesAsVectors_InWorldSpace(orange_edges, &logic.orange_F);
	logic.createEdgesAsVectors_InWorldSpace(yellow_edges, &logic.yellow_F);
	logic.createEdgesAsVectors_InWorldSpace(blue_edges, &logic.blue_F);
	logic.createEdgesAsVectors_InWorldSpace(green_edges, &logic.green_F);
	logic.createEdgesAsVectors_InWorldSpace(red_edges, &logic.red_F);
}

// Helper function to create a cube.
// type 0: x, 1 :y, 2: z.
// direction: 1 : positive, -1 : negative

void CreateCubeSide(BasicVertex* field, int type, int direction, Edge edges[4], Face face)
{
	float offset[3];
	//create 4 Triangles
	//with cube size

	//field[i] vertex position[i] x/y/z
	//schmeißt die Z-Achse raus
	for (int i = 0; i < 36; ++i)
		field[i].position[type] = direction * referenceBlock.size * 1.5f;	//direction vector //static face, entweder x,y oder z also bleibt die Seite unber�hrt und die Punkte verschieben sich nicht

	int localXDim = (type + 1) % 3;	//x? = y, y? = z, z? = x
	int localYDim = (type + 2) % 3;

	float blockSize = referenceBlock.size;

	float x = edges[0].vec.getX();
	float y = edges[0].vec.getY();

	float bottomX = edges[3].vec.getX();
	float bottomY = edges[3].vec.getY();

	//edges[0] - top
	//edges[1] - right
	//edges[2] - bottom
	//edges[3] - left
	
	//x direction
	for(int i = 0, j = 0; i < 9; i += 4, ++j){	// .0__ .4__ .8__	bis 
		//upper row
		field[i].position[localXDim] = x + blockSize * j;
		field[i+2].position[localXDim] = x + blockSize * j;
		
		//middle row
		field[i+12].position[localXDim] = x + blockSize * j;
		field[i+2+12].position[localXDim] = x + blockSize * j;

		//bottom row
		field[i+12+12].position[localXDim] = bottomX+ blockSize * j;
		field[i+2+12+12].position[localXDim] = bottomX + blockSize * j;

	}
	//x direction
	for(int i = 1, j = 1; i < 10; i += 4, ++j){		//__.1 __.5 __.9
		//upper row
		field[i].position[localXDim] = x + blockSize * j;
		field[i+2].position[localXDim] = x + blockSize * j;
		//middle row
		field[i+12].position[localXDim] = x + blockSize * j;
		field[i+2+12].position[localXDim] = x + blockSize * j;
		////bottom row
		field[i+12+12].position[localXDim] = bottomX + blockSize * j;
		field[i+2+12+12].position[localXDim] = bottomX + blockSize * j;
	}

	//.0__.1.4__.5.8__.9
	//.2/_.3.6/_.7.10/.11
	for(int i = 2, j = 0; i < 11; i+=4, j+=4){
		//upper row
		field[i].position[localYDim] = y + blockSize;
		field[i+1].position[localYDim] = y + blockSize;
		field[j].position[localYDim] = y;
		field[j+1].position[localYDim] = y;

		//middle row
		field[i+12].position[localYDim] = y + blockSize*2;
		field[i+1+12].position[localYDim] = y + blockSize*2;
		field[j+12].position[localYDim] = y + blockSize;
		field[j+1+12].position[localYDim] = y + blockSize;

		//bottom row
		field[i+12+12].position[localYDim] = bottomY + blockSize * 3;
		field[i+1+12+12].position[localYDim] = bottomY + blockSize * 3;
		field[j+12+12].position[localYDim] = bottomY + blockSize * 2;
		field[j+1+12+12].position[localYDim] = bottomY + blockSize * 2;
	}

	sprintf(text, "X Pos: %d, Y Pos: %d, Id: %d", edges[0].vec.getX(), edges[0].vec.getY(), edges[0].vec.getZ());

// Now the color.
	uint32_t baseColor;
	/*if (direction == 1)
		baseColor = color;
	else
		baseColor = 0x80;

	baseColor <<= type * 8;*/

	baseColor = face.middle.color;

	for (int i = 0; i < 6*36; ++i)
		field[i].color = baseColor;

	//offset
}

void savePointsIntoSliceArray(int axis, BasicVertex *field){

	for(int i = 0; i < 36; i+=12){
		s_slice_vertical_1[i] = field[i];
		s_slice_vertical_1[i+1] = field[i+1];
		s_slice_vertical_1[i+2] = field[i+2];
		s_slice_vertical_1[i+3] = field[i+3];
		field[i].flag = 1;	//rotate!
		field[i+1].flag = 1;
		field[i+2].flag = 1;
		field[i+3].flag = 1;
		field[i+1].color = 0x00000000;
		field[i+2].color = 0x00000000;
		field[i+3].color = 0x00000000;
	}
}

void drawPointsAndCreateCube(){

	// The vertices.
	int count = 0;
	int baseIndex = 0;
	float left = -1;
	float right = 1;

	//draw Faces

	//white
	float type = 2;	//z left/right
	float dir = left;	//left dependent on axis

	CreateCubeSide(&(s_basicVertices[count]), type, dir, white_edges, logic.white_F);
	// savePointsIntoSliceArray(0,&(s_basicVertices[count]));
	count+=36;	//one face further
	//yellow
	dir = right;
	CreateCubeSide(&(s_basicVertices[count]), type, dir, white_edges, logic.yellow_F);
	// savePointsIntoSliceArray(1,&(s_basicVertices[count]));
	count+=36;
	//red
	type = 1;
	dir = left;
	CreateCubeSide(&(s_basicVertices[count]), type, dir, white_edges, logic.red_F);
	// savePointsIntoSliceArray(2,&(s_basicVertices[count]));
	count+=36;
	//
	//orange
	dir = right;
	CreateCubeSide(&(s_basicVertices[count]), type, dir, white_edges, logic.orange_F);
	// savePointsIntoSliceArray(3,&(s_basicVertices[count]));
	count+=36;
	//blue
	type = 0;
	dir = left;
	CreateCubeSide(&(s_basicVertices[count]), type, dir, white_edges, logic.blue_F);
	// savePointsIntoSliceArray(4,&(s_basicVertices[count]));
	count+=36;
	dir = right;
	//green
	CreateCubeSide(&(s_basicVertices[count]), type, dir, white_edges, logic.green_F);
	// savePointsIntoSliceArray(5,&(s_basicVertices[count]));
	count = 0;
	
	//baseIndex = 0,1,2 Dreiecke 3 completes the quad

	for (int side = 0; side < 6 * 36; ++side)
	{
		s_basicIndices[count++] = baseIndex;
		s_basicIndices[count++] = baseIndex + 1;
		s_basicIndices[count++] = baseIndex + 2;

		s_basicIndices[count++] = baseIndex + 2;
		s_basicIndices[count++] = baseIndex + 3;
		s_basicIndices[count++] = baseIndex + 1;

		baseIndex += 4;
	}
}