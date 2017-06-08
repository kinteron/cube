#include "logic.h"
#include <iostream>
#include <cmath>
#include <vectormath.h>


using namespace sce::Vectormath::Simd::Aos;

void initFace(Face &f, float color) {
	Slice horizontal[3];
	Slice vertical[3];
	for (int j = 0; j < 3; ++j) {
		for (int i = 0; i < 3; ++i) {
			horizontal[j].triple[i].color = color;
			vertical[j].triple[i].color = color;
			horizontal[j].index = j;
			vertical[j].index = j;
		}
	}
	*f.horizontal = *horizontal;
	f.horizontal[1] = horizontal[1];
	f.horizontal[2] = horizontal[2];
	f.vertical[0] = vertical[0];
	f.vertical[1] = vertical[1];
	f.vertical[2] = vertical[2];
	
}



void Logic::initFaces() {

	//and referenceBlock
	referenceBlock.size = 0.4f;
	referenceBlock.offset = 0.1f;
	referenceBlock.color = 0x00000000;


	// construct this for every face... so 6 times with faces[i]
	// step 1 - define middleBlock for Face_**** e.g. white

	//color is defined as alpha 8bit blue 8bit green 8bit red

	// pointing to neighbour
	//1.
	white_F.bottomNeighbour = &orange_F;
	white_F.leftNeighbour = &blue_F;
	white_F.rightNeighbour = &green_F;
	white_F.topNeighbour = &red_F;
	//opposite = yellow
	white.color = 0xffffffff;		// 
	white_F.middle = white;		//|_| - white

	initFace(white_F, white.color);	//slice + color

	//2.
	orange_F.bottomNeighbour = &yellow_F;
	orange_F.leftNeighbour = &blue_F;
	orange_F.rightNeighbour = &green_F;
	orange_F.topNeighbour = &white_F;
	//opposite = red
	orange.color = 0xff0080FF;
	orange_F.middle = orange;

	initFace(orange_F, orange.color);	//slice + color

	//3.
	yellow_F.bottomNeighbour = &red_F;
	yellow_F.leftNeighbour = &blue_F;
	yellow_F.rightNeighbour = &green_F;
	yellow_F.topNeighbour = &orange_F;
	//opposite = white
	yellow.color = 0xff00CCDD;
	yellow_F.middle = yellow;

	initFace(yellow_F, yellow.color);	//slice + color

	//4.
	red_F.bottomNeighbour = &white_F;
	red_F.leftNeighbour = &blue_F;
	red_F.rightNeighbour = &green_F;
	red_F.topNeighbour = &yellow_F;
	//opposite = orange
	red.color = 0xff1111CC;
	red_F.middle = red;

	initFace(red_F, red.color);	//slice + color

	//5.
	blue_F.bottomNeighbour = &yellow_F;
	blue_F.leftNeighbour = &red_F;
	blue_F.rightNeighbour = &orange_F;
	blue_F.topNeighbour = &white_F;
	//opposite = green
	blue.color = 0xffAB2312;
	blue_F.middle = blue;

	initFace(blue_F, blue.color);	//slice + color

	//6.
	green_F.bottomNeighbour = &yellow_F;
	green_F.leftNeighbour = &orange_F;
	green_F.rightNeighbour = &red_F;
	green_F.topNeighbour = &white_F;
	//opposite = blue
	green.color = 0xff00AC00;
	green_F.middle = green;


	initFace(green_F, green.color);	//slice + color

	white.offset = orange.offset = blue.offset = green.offset = yellow.offset = red.offset = 0.1f;
	white.size = orange.size = blue.size = green.size = yellow.size = red.size = 0.4f;

}

void Logic::analyseFace(Face f) {
	for (int j = 0; j < 3; ++j){
		for (int i = 0; i < 3; ++i) {
			cout << f.horizontal[j].triple[i].color << endl;
		}
	}
}

void Logic::assignEdgesToFace(Face &f, Edge edge[]) {
	////pointer = pointer
	//f.top = edge[0];
	//f.right = edge[1];
	//f.bottom = edge[2];
	//f.left = edge[3];
}


void Logic::createEdgesAsVectors_InWorldSpace(Edge* edges, Face *face) {	//edges to save edges into it, face for which edges are created
	
	float vectorLength = referenceBlock.size * 3.f;
	float half = vectorLength / 2;
	Block *above = new Block[3];
	Block *below = new Block[3];	/*

									DELETE

									
									*/
									//construct face by putting on edges around the face 
									//top


	Vector3 vek1 = Vector3(-half, -half, -half);	//fix number in clippingSpace
	//touchSpace ist oben links 0,0 - alles wie text, etc. in displaySpace
	//set color for face and faceTop along edge
	for (int i = 0; i < 3; ++i) {
		below[i].color = face->middle.color;
		above[i].color = face->topNeighbour->middle.color;
	}

	Edge top = { vek1, *above, *below };	//give over array
	edges[0] = top;
	face->top = top;

	//renderEdge
	
	//right
	Vector3 vek2 = Vector3(vek1.getX().getAsFloat() + vectorLength, vek1.getY().getAsFloat(), vek1.getZ().getAsFloat());
	//set color for face and faceRight along edge
	for (int i = 0; i < 3; ++i) {
		above[i].color = face->rightNeighbour->middle.color;
		//below stays the same
	}
	
	Edge right = { vek2, *above, *below };
	edges[1] = right;
	face->right = right;

	//bottom
	Vector3 vek3 = Vector3( vek2.getX().getAsFloat(), vek2.getY().getAsFloat(), vek1.getZ().getAsFloat());
	vek3.setY(vek2.getY() + vectorLength);
	//set color for face and faceBottom along edge
	for (int i = 0; i < 3; ++i) {
		above[i].color = face->bottomNeighbour->middle.color;
		//below stays the same
	}

	Edge bottom = { vek3, *above, *below };
	edges[2] = bottom;
	face->bottom = bottom;

	//left
	Vector3 vek4 = Vector3( vek3.getX().getAsFloat()-vectorLength, vek3.getY().getAsFloat(), vek3.getZ());
	//set color for face and faceLeft along edge
	for (int i = 0; i < 3; ++i) {
		above[i].color = face->leftNeighbour->middle.color;
		//below stays the same
	}

	Edge left = { vek4, *above, *below };
	edges[3] = left;
	face->left = left;

}


void rotateLeft(Face &face) {
	
}
void rotateRight(Face &face, Slice horizontal) {

	

}

void Logic::rotateUp(Face *face, Slice vertical) {
	

	//cout << face->vertical[2].triple[rand() * 2].color << endl;
	
	
	cout << "original " << endl;
	cout << face->vertical[vertical.index].triple[0].color << endl;
	cout << face->vertical[vertical.index].triple[1].color << endl;
	cout << face->vertical[vertical.index].triple[2].color << endl;
	cout << vertical.index << endl;

	cout << "Nachbar: " << endl;
	cout << face->bottomNeighbour->vertical[vertical.index].triple[0].color << endl;
	cout << face->bottomNeighbour->vertical[vertical.index].triple[1].color << endl;
	cout << face->bottomNeighbour->vertical[vertical.index].triple[2].color << endl;
	cout << "Swapen: " << endl;

	Slice * temp;
	temp = &face->bottomNeighbour->vertical[vertical.index];
	face->vertical[vertical.index] = *temp;
	
	cout << face->vertical[vertical.index].triple[0].color << endl;
	cout << face->vertical[vertical.index].triple[1].color << endl;
	cout << face->vertical[vertical.index].triple[2].color << endl;


}
void rotateBottom(Face &face) {

}

void rotateQuaternion(float* quat, float* rotationVelocity){
}

void clearArrays(){

}