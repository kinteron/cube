#pragma once
#include <math.h>
#include <assert.h>
#include <iostream>
#include <vectormath.h>

using namespace sce::Vectormath::Simd::Aos;
using namespace std;


struct Block {
	unsigned long color;
	float size;
	float offset;
};

struct Edge {
	Vector3 vec;
	Block above[3];
	Block below[3];
};

struct Slice {
	Block triple[3];
	int index;
};

struct Face {
	Face* leftNeighbour;
	Face* rightNeighbour;
	Face* bottomNeighbour;
	Face* topNeighbour;

	Block middle;
	
	Slice horizontal[3];	//top->bottom
	Slice vertical[3];		//left->right

	Edge top;
	Edge bottom;
	Edge left;
	Edge right;
};

static Block referenceBlock;
static const float sizeFactor = 0.1f;

class Logic{

public:

	//float magnitude(Vector3 &vec);
	void initFaces();
	void createSlicesInFace(Face &face);
	//void assignBlocksToSlice(Slice &slice, );
	void createEdgesAsVectors_InWorldSpace(Edge* edges, Face *face);
	void assignEdgesToFace(Face &f, Edge edge[]);
	void clearArrays();
	void rotateLeft(Face &face);
	void rotateRight(Face &face);
	void rotateUp(Face *face, Slice slice);
	void rotateBottom(Face &face);
	void analyseFace(Face f);
	

	Face white_F, green_F, yellow_F, orange_F, blue_F, red_F;

	Block white, green, yellow, orange, blue, red;

	Edge top, right, bottom, left;

	
};