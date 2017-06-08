/* SCE CONFIDENTIAL
 * PlayStation(R)Vita Programmer Tool Runtime Library Release 02.000.081
 * Copyright (C) 2010 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

 // All fonts related stuff hs been ripped out.

 /*

	 This sample shows how to initialize libdbgfont (and libgxm),
	 and render debug font with triangle for clear the screen.

	 This sample is split into the following sections:

		 1. Initialize libdbgfont
		 2. Initialize libgxm
		 3. Allocate display buffers, set up the display queue
		 4. Create a shader patcher and register programs
		 5. Create the programs and data for the clear
		 6. Start the main loop
			 7. Update step
			 8. Rendering step
			 9. Flip operation and render debug font at display callback
		 10. Wait for rendering to complete
		 11. Destroy the programs and data for the clear triangle
		 12. Finalize libgxm
		 13. Finalize libdbgfont

	 Please refer to the individual comment blocks for details of each section.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sceerror.h>

#include <touch.h>

#include <libdbg.h>
#include <libdbgfont.h>

#include <math.h>
#include <vectormath.h>		//matrix4, vector3, etc.

#include "gxm.h"		//prepares gxm usage
#include "logic.h"		//is cube logic
#include "helper.h"		//contains variable declarations
#include "control.h"	//handles input

using namespace sce::Vectormath::Simd::Aos;


//static int s_selector = 0;

static float s_accumulatedRotationAngle;	// accumulated 

//!! The program parameter for the transformation of the triangle and the color.
static float s_positionalData[2];

static float s_accumulatedTurningAngleX;
static float s_accumulatedTurningAngleY;

//Quaternion zum Interpolieren
static Quat s_accumulatedQuaternion = Quat::identity();



/*	@brief Main entry point for the application
	@return Error code result of processing during execution: <c> SCE_OK </c> on success,
	or another code depending upon the error
*/

/* drawing
			36
	
*/


int main(void);

// prototypes
// !! Here we create the matrix.
void Update(void);

void updateSide(BasicVertex*, int, int);
void keyInput(SceCtrlData buf);
void checkSide(BasicVertex *field, int currentAxis, int dir);

//helper methods

float makeFloat(unsigned char input)
{
	// convert into display coordinates
	return (((float)(input)) / 255.0f * 2.0f) - 1.0f;
}

void keyInput(SceCtrlData buffer){

	//Joystick Control
	//explicit initialization is not necessary - runs in a default module
	//use ctrl.h

	SceCtrlData data;	//input for
	
	float dir = -1.f;
	int type = 1;	//rotiert eigentlich immer um Z
	int count = 0;

	if (isButtonPressed(SCE_CTRL_CROSS) != 0) {
		if(!isPressed){
			std::cout << data.lx << std::endl;
			printf("", data.ly);
			char buf[32];
			isPressed = true;
		}
	}else{
		isPressed = false;
	}
		//ausfindig machen welche Punkte
		updateSide(&(s_basicVertices[count]), type, dir);

	if(isButtonPressed(SCE_CTRL_L) != 0){
		//rotateLeftSlice();
		sceDbgFontPrint(0, 0, 0xffffff, "sd");
		std::cout << data.lx << std::endl;
		//printf("",data.ly);
		
		snprintf(buf, sizeof(buf), data.rx + "");
		
		printf(data.lx + "");
		printf(data.ry + "");
	}
	if(isButtonPressed(SCE_CTRL_R) != 0){
		//rotateRightSlice();
		//snprintf(buf, sizeof(buf), data.rx + "");
		sceDbgFontPrint(0, 0, 0xffffff, "R");
	}

	//select Vertices
	if((buffer.buttons & SCE_CTRL_RIGHT) > 0){
		
		//control which points on axis
		//selectVertices(s_selector, s_slice_vertical_1);

		checkSide(&(s_basicVertices[count]), type, dir);
	}

	if((buffer.buttons & SCE_CTRL_LEFT) > 0){
		//selectVertices(s_selector, s_slice_vertical_1);
	}
}

//texture vars
void* gxt;
int            err = SCE_OK;
unsigned int   nbrTexturesInGXTFile;
unsigned int   textureDataSize;
const void     *textureDataPointer;
SceGxmTexture  texture;
unsigned int   textureIndexToUse = 0;
SceUID texUID;

void initTexture(){
	//load file
	SceUID texFileID = sceIoOpen("app0:/tex.gxt", SCE_O_RDONLY, 0);
	assert(texFileID >= 0);
	SceOff size = sceIoLseek(texFileID, 0, SCE_SEEK_END);
	
	int start = sceIoLseek(texFileID, 0, SCE_SEEK_SET);
	void* texData = malloc(size);
	assert(sceIoRead(texFileID, texData, size) >= 0);

	//create texture from mem
	// Extract info from GXT file buffer{
    err = sceGxtCheckData(texData);
    assert(err == SCE_OK);

    // GXT files can contain more than one texture.
    // Get texture count from GXT file.
    nbrTexturesInGXTFile = sceGxtGetTextureCount(texData);
    assert(nbrTexturesInGXTFile > 0);

    // Get texture data size within the GXT file buffer
    textureDataSize = sceGxtGetDataSize(texData);
    assert(textureDataSize);

	gxt = graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, textureDataSize, SCE_GXM_TEXTURE_ALIGNMENT, SCE_GXM_MEMORY_ATTRIB_READ, &texUID);
	memcpy(gxt, texData, textureDataSize);

    err = sceGxtInitTexture(&texture,    // Texture structure to be filled out
                            texData,    // Pointer to GXT file buffer
                            gxt,    // Pointer to the actual texture data
                            0);    // Texture index within GXT file buffer
    assert(err == SCE_OK);
	//sceGxmTextureSetMinFilter(&texture, SCE_GXM_TEXTURE_FILTER_LINEAR);
	//sceGxmTextureSetMagFilter(&texture, SCE_GXM_TEXTURE_FILTER_LINEAR);
	sceGxmSetFragmentTexture(s_context, 0, &texture);

	free(texData);
}

/* Main entry point of program */
int main(void)
{
	logicBuild();
	int returnCode = SCE_OK;

	/* initialize libdbgfont and libgxm */
	returnCode = initGxm();
	SCE_DBG_ALWAYS_ASSERT(returnCode == SCE_OK);

	SceDbgFontConfig config;
	memset(&config, 0, sizeof(SceDbgFontConfig));
	config.fontSize = SCE_DBGFONT_FONTSIZE_LARGE;

	returnCode = sceDbgFontInit(&config);
	SCE_DBG_ALWAYS_ASSERT(returnCode == SCE_OK);

	/* Message for SDK sample auto test */
	printf("## simple: INIT SUCCEEDED ##\n");

	/*before any GxmData*/
	
	/* create gxm graphics data */
	createGxmData();

	// Set sampling mode for input device.
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG_WIDE);

	// texture
	initTexture();

	drawPointsAndCreateCube();
	//savePointsIntoSliceArray(0, &(s_basicVertices[rowCount]);	//vertical

	/* 6. main loop */
	while (true)
	{
		Update();
		render();
		//sceDbgFontPrint(20, 20, 0xffffffff, "Hello World");
		cycleDisplayBuffers();
	}

	/*
	// 10. wait until rendering is done
	sceGxmFinish( s_context );

	// destroy gxm graphics data
	destroyGxmData();

	// shutdown libdbgfont and libgxm
	returnCode = shutdownGxm();
	SCE_DBG_ALWAYS_ASSERT( returnCode == SCE_OK );

	// Message for SDK sample auto test
	printf( "## api_libdbgfont/simple: FINISHED ##\n" );

	return returnCode;
	*/
}

void selectVertices(int index, BasicVertex *field){
	assert(sizeof(field) > 0);
	for(int i = 0; i < 12; ++i){
		field[i].color = 0x000000;
	}
}

void checkSide(BasicVertex *field, int currentAxis, int dir){
	//referenceBlock size einprogrammieren
	for(int i = 0; i < 9*4*6; i++){
		if(field[i].position[currentAxis] >= (0.5f - (2.0f * 0.5f / 3.0f)) && dir == -1){
			field[i].flag = 1;
		}else if(field[i].position[currentAxis] <= -(0.5f - (2.0f * 0.5f / 3.0f)) && dir == 1)
			field[i].flag = 1;
		else if (field[i].position[currentAxis] > -(0.5f - (2.0f * 0.5f / 3.0f)) && field[i].position[currentAxis] < (0.5f - (2.0f * 0.5f / 3.0f)) && dir == 0){
			field[i].flag = 1;
		}
	}
}

void selectVertices(BasicVertex *field, int currentAxis, int dir){
	
	for(int i = 0; i < 12; i++){

	}
	// s_slice_horizontal_1 = 
}

void updateSide(BasicVertex* field, int type, int direction){
	//type = axis, which the points are getting rotated
	
	//local coordinates of face
	int localXDim = (type + 1) % 3;	//x? = y, y? = z, z? = x
	int localYDim = (type + 2) % 3;
	int z = 2;

	/*for(int i = 0; i < sizeof(field) / sizeof(field[0]); ++i){*/
	
	Matrix4 rotation;
	float rotatedX,rotatedY;
	switch(type){
		case 0 : 
			rotation = Matrix4::rotationZYX(Vector3(s_accumulatedRotationAngle, 0.0f, 0.0f));	//3x3 Matrix with 90* rotation around X
		break;
		case 1 : 
			rotation = Matrix4::rotationZYX(Vector3(0.0f,s_accumulatedRotationAngle, 0.0f));
		break;
		case 2 : 
			rotation = Matrix4::rotationZYX(Vector3(0.0f, 0.0f,s_accumulatedRotationAngle));
		break;
	}

	s_rotationMatrix = rotation;
	
	//linkes face 
	for(int i = 0; i < 36; ++i){
		/*Vector3 vec = Vector3(field[i].position[0], field[i].position[1], field[i].position[z]);
		vec = rotation * vec;
		field[i].position[0] = vec.getX();
		field[i].position[1] = vec.getY();
		field[i].position[z] = vec.getZ();*/
		field[i].flag = 1;
	}

	/*
	TODO
	rotate around direction * 1.5f * blockSize		somehow
	//save points into vector multiply with matrix, then resave points (from vector) into array 
	*/

	//selected Vertices into rotatePoints

}

void clearRotFlag(BasicVertex cube[]){
	for(int i = 0; i < 9*4*6; i++){
			cube[i].flag = 0;
	}
}

Quat setQuaternion(){
	Quat quaternion = Quat::Quat(s_accumulatedTurningAngleY, -s_accumulatedTurningAngleX,  0.0f, 0.0f);
	return quaternion;
}

void touchInput() {

	//SceTouchData* pData;
	SceUInt32 nBufs = 1;	//sets of touchData returned in pData

	//for analog pad 
	SceCtrlData buffer;
	sceCtrlReadBufferPositive(0, &buffer, 1);

	//open touchPad
	SceUInt32 state = SCE_TOUCH_SAMPLING_STATE_START;
	//second input
	SceUInt32 backPad = SCE_TOUCH_PORT_BACK;
	SceUInt32 frontPad = SCE_TOUCH_PORT_FRONT;

	sceTouchSetSamplingState(backPad, state);
	sceTouchSetSamplingState(frontPad, state);

	//read input
	SceTouchData backTouch, frontTouch;
	
	sceTouchRead(backPad, &backTouch, nBufs);
	sceTouchRead(frontPad, &frontTouch, 1);
	
	float factor = 0.0f;
	if(makeFloat(buffer.lx) > .5f){
		s_accumulatedTurningAngleX += makeFloat(buffer.lx)*0.1f;
		factor = 0.05f;
	}
	if(makeFloat(buffer.lx) < -.5f){
		s_accumulatedTurningAngleX += makeFloat(buffer.lx)*0.1f;
			factor = 0.05f;
	}
	if(makeFloat(buffer.ly) > .5f){
		s_accumulatedTurningAngleY += makeFloat(buffer.ly)*0.1f;
			factor = 0.05f;
	}
	if(makeFloat(buffer.ly) < -.5f){
		s_accumulatedTurningAngleY += makeFloat(buffer.ly)*0.1f;
		factor = 0.05f;
	}
	s_accumulatedTurningAngleX = makeFloat(buffer.lx);
	s_accumulatedTurningAngleY = makeFloat(buffer.ly);
	
	if(buffer.rx > 0.01f || buffer.ry > 0.01f)
		//referenceBlock.size += sizeFactor;

	//rightCursor
	if(makeFloat(buffer.rx) > .5f){	//needs blocksize + offset to work properly @ref logic-init, createCube()
		s_accumulatedRotationAngle += makeFloat(buffer.rx) * 0.1f;
	}else{
		for(int i = 0; i < 6*36; ++i){	//rotates when cursor is pressed turns off when releasing cursor
			s_basicVertices[i].flag = 0;
		}
	}

	/*if(makeFloat(buffer.ry) < -.5f){
		s_accumulatedRotationAngle += makeFloat(buffer.ry) * 0.1f;
	}
*/
 	float x = (backTouch.report[0].x);
	float y = (backTouch.report[0].y);
	float x2 = (backTouch.report[1].x);
	float y2 = (backTouch.report[1].y);
	float length1 = sqrtf(makeFloat(x) * makeFloat(x) + makeFloat(y )* makeFloat(y));
	float length2 = sqrtf(makeFloat(x2) * makeFloat(x2) + makeFloat(y2) * makeFloat(y2));
	length1 = length1;
	length2 = length2;
	float oldDragX = 0;
	float oldDragY = 0;

	if(backTouch.reportNum > 0){
		float distanceX = makeFloat(x) - makeFloat(x2);	//umrechnung touch in clip
		s_accumulatedTurningAngleX = 0.002f * length1 + distanceX;
		if(backTouch.reportNum > 1){
			float distanceY = makeFloat(y) - makeFloat(y2);
			s_accumulatedTurningAngleY = length2 * 0.002f + distanceY;
		}
		factor = 0.05f;
	}
	oldDragX = x - x2;
	oldDragY = y - y2;
	//raycast on minicube
	float oldTouchX = makeFloat(frontTouch.report[0].x);
	float oldTouchY = makeFloat(frontTouch.report[0].y);
	float length = 0;
	//mindestens ein touch-Punkt?
	if(frontTouch.reportNum > 0){
		length = (oldTouchX-makeFloat(frontTouch.report[0].x)) * (oldTouchX-makeFloat(frontTouch.report[0].x)) + (oldTouchY-makeFloat(frontTouch.report[0].y)) * (oldTouchY-makeFloat(frontTouch.report[0].y));
	}//loslassen

	if(length > 10){
		sceDbgFontPrint(0, 0, 0xffffff, "hallo");
	}

	//rotation um x-Achse mit Matrix4
	//Matrix4 rotation = Matrix4::rotationZYX(Vector3(s_accumulatedTurningAngleY, s_accumulatedTurningAngleX, 0.0f));

	Matrix4 rotation = Matrix4::rotationZYX(Vector3(s_accumulatedTurningAngleY, -s_accumulatedTurningAngleX, 0.0f));
	//Rotation mit Quaternion ermöglicht 360* Drehung
	//Quaternion = vektor * Winkel_als Quaternion
	//in Form ZYX (wie in Matrix4)	mit Realteil hinten = 0
	
	//accumuliertes Quaternion
	//neues Quaternion ergbit sich aus Winkel mal altem Quaternion
	Quat quatAngle = setQuaternion();
	//s_accumulatedQuaternion = s_accumulatedQuaternion + factor * (s_accumulatedQuaternion * quatAngle * conj(s_accumulatedQuaternion));
	s_accumulatedQuaternion = s_accumulatedQuaternion + factor * (quatAngle * s_accumulatedQuaternion);	//euler-step
	s_accumulatedQuaternion = normalize(s_accumulatedQuaternion);	//einheitsquaternion

	rotation = Matrix4::rotation(s_accumulatedQuaternion);
	Matrix4 lookAt = Matrix4::lookAt(Point3(0.0f, 0.0f, -3.0f), Point3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
	Matrix4 perspective = Matrix4::perspective(PI / 4.0f,
		(float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT,
		0.1f,
		10.0f);

	s_finalTransformation = perspective * lookAt * rotation;
	
	//Quat quat = Quat.identity;
	keyInput(buffer);	// call by value
}

void Update(void)
{
	touchInput();
};

int isButtonPressed(int button) {

	//controller
	SceCtrlData ct;
	int res;
	//port 0, buffer, nBuffs(number of buffers to receive controller data(1-64)
	res = sceCtrlReadBufferPositive(0, &ct, 1);	//
	if (res < 0) {
		return (res);
	}
	if ((ct.buttons & SCE_CTRL_CROSS) != 0) {
		return 1;
	}
	else {
		return 0;
	}
}

/* Main render function */
void render(void)
{
	/* render libgxm scenes */
	renderGxm();
	//myLogic

}