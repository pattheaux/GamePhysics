#include "BlobGameSystem.h"

BlobGameSystem::BlobGameSystem(Camera* camera)
{
	mCanRestart = true;
	mDrawDebug = true;
	mCanDebug = true;
	mCamera = camera;
	mBlobs = vector<Blob*>();
	mBlocks = vector<Block*>();
	mParticleSystem = new ParticleSystem();

	createObjects();
	initBlobs();
	initBlocks();
	applyForces();
	applyContacts();

	mCamera->savePos();
}

BlobGameSystem::~BlobGameSystem()
{
	cleanUp();
}

void BlobGameSystem::cleanUp()
{
	delete(mParticleSystem);
}

void BlobGameSystem::createObjects()
{
	Blob* player = createBlob("Player", Color(0, 1, 0), 3, 2, Vector3(0, 15, 0));
	Blob* arm1 = createBlob("Arm1", Color(0, 0.75f, 0), 2, 0.1f, Vector3(10, 20, -10));
	Blob* arm2 = createBlob("Arm2", Color(0, 0.75f, 0), 2, 0.1f, Vector3(-10, 20, -10));
	Blob* arm3 = createBlob("Arm3", Color(0, 0.75f, 0), 2, 0.1f, Vector3(0, 20, 10));

	mPlayer = player;
	mPlayer->particle->limitVelocity(true, Vector3(100,100,100));
	mPlayerSpeed = 100;
	createCable(player->particle, arm1->particle, Color(0,1,0), 0.5f, 20);
	createCable(player->particle, arm2->particle, Color(0,1,0), 0.5f, 20);
	createCable(player->particle, arm3->particle, Color(0,1,0), 0.5f, 20);

	mBlobs.push_back(player);
	mBlobs.push_back(arm1);
	mBlobs.push_back(arm2);
	mBlobs.push_back(arm3);

	//createPyramid("Pyramid1", Color(1,0,0), 2, 1, 20, Vector3(-20, 20, 0));
	//createCube("Cube1", Color(1,0,0), 2, 1, 10, Vector3(20, 20, 0));
	//createDiamond("Diamond1", Color(1,0,0), 2, 1, 10, Vector3(0, 20, 0));

	Block* floor = createBlock("Floor", Color(0.2f, 0.2f, 0.2f), 100, 100, 1, -1, Vector3(0,0,0));

	mBlocks.push_back(floor);

	
	//applyForces();
	//applyContacts();
}

Blob* BlobGameSystem::createBlob(string name, Color color, double size, double mass, Vector3 pos)
{
	Blob* blob = new Blob();
	blob->name = name;
	blob->color = color;
	blob->particle = new SphereParticle(size, mass, blob->color);
	blob->startPosition = pos;

	return blob;
}

Block* BlobGameSystem::createBlock(string name, Color color, float width, float length, float height, double mass, Vector3 pos)
{
	Block* block = new Block();
	block->name = name;
	block->color = color;
	block->particle = new RectParticle(width, length, height, mass, block->color);
	block->startPosition = pos;

	return block;
}

void BlobGameSystem::createCube(string name, Color color, double size, double mass, double length, Vector3 pos)
{
	float posOffset = length/2;
	float beamLength = sqrt(pow(length,2)*2);
	Blob* cubeBUL = createBlob(name+"BottomUpperLeft", color, size, mass, Vector3(pos.x-posOffset, pos.y-posOffset, pos.z-posOffset));
	Blob* cubeBUR = createBlob(name+"BottomUpperRight", color, size, mass, Vector3(pos.x+posOffset, pos.y-posOffset, pos.z-posOffset));
	Blob* cubeBLL = createBlob(name+"BottomLowerLeft", color, size, mass, Vector3(pos.x-posOffset, pos.y-posOffset, pos.z+posOffset));
	Blob* cubeBLR = createBlob(name+"BottomLowerRight", color, size, mass, Vector3(pos.x+posOffset, pos.y-posOffset, pos.z+posOffset));
	Blob* cubeTUL = createBlob(name+"TopUpperLeft", color, size, mass, Vector3(pos.x-posOffset, pos.y+posOffset, pos.z-posOffset));
	Blob* cubeTUR = createBlob(name+"TopUpperRight", color, size, mass, Vector3(pos.x+posOffset, pos.y+posOffset, pos.z-posOffset));
	Blob* cubeTLL = createBlob(name+"TopLowerLeft", color, size, mass, Vector3(pos.x-posOffset, pos.y+posOffset, pos.z+posOffset));
	Blob* cubeTLR = createBlob(name+"TopLowerRight", color, size, mass, Vector3(pos.x+posOffset, pos.y+posOffset, pos.z+posOffset));

	createRod(cubeBUL->particle, cubeBUR->particle, color, length);
	createRod(cubeBUR->particle, cubeBLR->particle, color, length);
	createRod(cubeBLR->particle, cubeBLL->particle, color, length);
	createRod(cubeBLL->particle, cubeBUL->particle, color, length);

	createRod(cubeTUL->particle, cubeTUR->particle, color, length);
	createRod(cubeTUR->particle, cubeTLR->particle, color, length);
	createRod(cubeTLR->particle, cubeTLL->particle, color, length);
	createRod(cubeTLL->particle, cubeTUL->particle, color, length);

	createRod(cubeTUL->particle, cubeBUL->particle, color, length);
	createRod(cubeTUR->particle, cubeBUR->particle, color, length);
	createRod(cubeTLR->particle, cubeBLR->particle, color, length);
	createRod(cubeTLL->particle, cubeBLL->particle, color, length);
	
	//Roof
	createRod(cubeTUL->particle, cubeTLR->particle, color, beamLength);
	createRod(cubeTUR->particle, cubeTLL->particle, color, beamLength);
	//Floor
	createRod(cubeBUL->particle, cubeBLR->particle, color, beamLength);
	createRod(cubeBUR->particle, cubeBLL->particle, color, beamLength);
	//Front wall
	createRod(cubeTLL->particle, cubeBLR->particle, color, beamLength);
	createRod(cubeTLR->particle, cubeBLL->particle, color, beamLength);
	//Back wall
	createRod(cubeTUL->particle, cubeBUR->particle, color, beamLength);
	createRod(cubeTUR->particle, cubeBUL->particle, color, beamLength);
	//Left wall
	createRod(cubeTUL->particle, cubeBLL->particle, color, beamLength);
	createRod(cubeTLL->particle, cubeBUL->particle, color, beamLength);
	//Right wall
	createRod(cubeTUR->particle, cubeBLR->particle, color, beamLength);
	createRod(cubeTLR->particle, cubeBUL->particle, color, beamLength);

	mBlobs.push_back(cubeBUL);
	mBlobs.push_back(cubeBUR);
	mBlobs.push_back(cubeBLR);
	mBlobs.push_back(cubeBLL);

	mBlobs.push_back(cubeTUL);
	mBlobs.push_back(cubeTUR);
	mBlobs.push_back(cubeTLR);
	mBlobs.push_back(cubeTLL);
}

void BlobGameSystem::createPyramid(string name, Color color, double size, double mass, double length, Vector3 pos)
{
	float posOffset = length/2;
	//float bottomMutliplier = 1.0f;
	float topMultiplier = 1.1f;
	Blob* pyramidU = createBlob(name+"Upper", color, size, mass, Vector3(pos.x, pos.y, pos.z-posOffset));
	Blob* pyramidLL = createBlob(name+"LowerLeft", color, size, mass, Vector3(pos.x-posOffset, pos.y, pos.z+posOffset));
	Blob* pyramidLR = createBlob(name+"LowerRight", color, size, mass, Vector3(pos.x+posOffset, pos.y, pos.z+posOffset));
	Blob* pyramidTop = createBlob(name+"Top", color, size, mass, Vector3(pos.x, pos.y+length, pos.z));

	createRod(pyramidU->particle, pyramidLR->particle, color, length);
	createRod(pyramidLR->particle, pyramidLL->particle, color, length);
	createRod(pyramidLL->particle, pyramidU->particle, color ,length);

	createRod(pyramidTop->particle, pyramidLR->particle, color, length*topMultiplier);
	createRod(pyramidTop->particle, pyramidLL->particle, color, length*topMultiplier);
	createRod(pyramidTop->particle, pyramidU->particle, color, length*topMultiplier);

	mBlobs.push_back(pyramidU);
	mBlobs.push_back(pyramidLL);
	mBlobs.push_back(pyramidLR);
	mBlobs.push_back(pyramidTop);
}

void BlobGameSystem::createDiamond(string name, Color color, double size, double mass, double length, Vector3 pos)
{
	float posOffset = length/2;
	float beamLength = length*1.1f;
	Blob* diamondU = createBlob(name+"Upper", color, size, mass, Vector3(pos.x, pos.y, pos.z-posOffset));
	Blob* diamondLL = createBlob(name+"LowerLeft", color, size, mass, Vector3(pos.x-posOffset, pos.y, pos.z+posOffset));
	Blob* diamondLR = createBlob(name+"LowerRight", color, size, mass, Vector3(pos.x+posOffset, pos.y, pos.z+posOffset));
	Blob* diamondTop = createBlob(name+"Top", color, size, mass, Vector3(pos.x, pos.y+length, pos.z));
	Blob* diamondBottom = createBlob(name+"Top", color, size, mass, Vector3(pos.x, pos.y-length, pos.z));

	createRod(diamondU->particle, diamondLR->particle, color, length);
	createRod(diamondLR->particle, diamondLL->particle, color, length);
	createRod(diamondLL->particle, diamondU->particle, color ,length);

	createRod(diamondTop->particle, diamondLR->particle, color, beamLength);
	createRod(diamondTop->particle, diamondLL->particle, color, beamLength);
	createRod(diamondTop->particle, diamondU->particle, color, beamLength);

	createRod(diamondBottom->particle, diamondLR->particle, color, beamLength);
	createRod(diamondBottom->particle, diamondLL->particle, color, beamLength);
	createRod(diamondBottom->particle, diamondU->particle, color, beamLength);

	mBlobs.push_back(diamondU);
	mBlobs.push_back(diamondLL);
	mBlobs.push_back(diamondLR);
	mBlobs.push_back(diamondTop);
	mBlobs.push_back(diamondBottom);
}

void BlobGameSystem::applyForces()
{
	//mParticleSystem->addParticle(mBlobs[0]->particle);
	EarthGravityGenerator* earthGrav = new EarthGravityGenerator();

	vector<Blob*>::iterator blobIt = mBlobs.begin();

	for (blobIt; blobIt != mBlobs.end(); ++blobIt)
	{
		mParticleSystem->applyForce((*blobIt)->particle, earthGrav);
	}
}

void BlobGameSystem::applyContacts()
{
	/*ParticleRod* rod = new ParticleRod();
	rod->length = 20;

	rod->particle[0] = mBlobs[0]->particle;
	rod->particle[1] = mBlobs[1]->particle;
	mParticleSystem->addContact(rod);*/
	/*ParticleCable* cable = new ParticleCable();
	cable->maxLength = 15;
	cable->restitution = 0.3f;

	cable->particle[0] = mBlocks[0]->particle;
	cable->particle[1] = mBlobs[1]->particle;
	mParticleSystem->addContact(cable);*/

	vector<Block*>::iterator blockIt = mBlocks.begin();

	for (blockIt; blockIt != mBlocks.end(); ++blockIt)
	{
		mParticleSystem->addParticle((*blockIt)->particle);
		vector<Blob*>::iterator blobIt = mBlobs.begin();
		for (blobIt; blobIt != mBlobs.end(); ++blobIt)
		{
			GroundContactGenerator* groundGen = new GroundContactGenerator();
			groundGen->ground = (*blockIt)->particle;
			groundGen->other = (*blobIt)->particle;
			groundGen->radius = (*blobIt)->particle->getRadius();
			//mParticleSystem->applyContact((*blobIt)->particle, (*blockIt)->particle, groundGen);
			mParticleSystem->addContact(groundGen);
		}
	}
}

void BlobGameSystem::createRod(Particle* particle1, Particle* particle2, Color color, double length)
{
	ParticleRod* rod = new ParticleRod();
	rod->length = length;
	rod->lineColor = color;
	rod->particle[0] = particle1;
	rod->particle[1] = particle2;
	mParticleSystem->addContact(rod);
}

void BlobGameSystem::createCable(Particle* particle1, Particle* particle2, Color color, double restitution, double length)
{
	ParticleCable* cable = new ParticleCable();
	cable->restitution = restitution;
	cable->maxLength = length;
	cable->lineColor = color;
	cable->particle[0] = particle1;
	cable->particle[1] = particle2;
	mParticleSystem->addContact(cable);
}

void BlobGameSystem::movePlayer(Vector3 moveVelocity)
{
	if (mPlayer == NULL)
	{
		return;
	}

	mPlayer->particle->addForce(moveVelocity);
}

void BlobGameSystem::drawDebug()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	glColor4f(DEBUG_COLOR.r, DEBUG_COLOR.g, DEBUG_COLOR.b, DEBUG_COLOR.a);
	drawString(-1, 0.95f, "Hand over your coffee!", DEBUG_COLOR);
	drawString(-1, 0.9f, "Blob pos: "+mBlobs[0]->particle->getPosition().print(), DEBUG_COLOR);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void BlobGameSystem::drawString(float x, float y, string text, Color color)
{
	const char* temp = text.c_str();

	glRasterPos2f(x, y);

	char buf[300];
	sprintf(buf, temp);

	const char *p = buf;

	glColor4f(color.r, color.g, color.b, color.a);

	do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
	while (*(++p));
}

void BlobGameSystem::initBlobs()
{
	vector<Blob*>::iterator blobIt = mBlobs.begin();

	for (blobIt; blobIt != mBlobs.end(); ++blobIt)
	{
		(*blobIt)->particle->setPosition((*blobIt)->startPosition);
		(*blobIt)->particle->setVelocity(Vector3(0,0,0));
	}
}

void BlobGameSystem::initBlocks()
{
	vector<Block*>::iterator blockIt = mBlocks.begin();

	for (blockIt; blockIt != mBlocks.end(); ++blockIt)
	{
		(*blockIt)->particle->setPosition((*blockIt)->startPosition);
		(*blockIt)->particle->setVelocity(Vector3(0,0,0));
	}
}

void BlobGameSystem::press(char key, int isPressed)
{
	if (key == 'r')
	{
		if (isPressed)
		{
			if (mCanRestart)
			{
				initBlobs();
				mCanRestart = false;
			}
		}
		else
		{
			mCanRestart = true;
		}
	}

	if (key == 'f')
	{
		if (isPressed)
		{
			if (mCanDebug)
			{
				mDrawDebug = !mDrawDebug;
				mCanDebug = false;
			}
		}
		else
		{
			mCanDebug = true;
		}
	}

	if (key == 'w')
	{
		movePlayer(Vector3(0,0,-mPlayerSpeed));
	}
	if (key == 'a')
	{
		movePlayer(Vector3(-mPlayerSpeed,0,0));
	}
	if (key == 's')
	{
		movePlayer(Vector3(0,0,mPlayerSpeed));
	}
	if (key == 'd')
	{
		movePlayer(Vector3(mPlayerSpeed,0,0));
	}
}

void BlobGameSystem::update(float duration)
{
	mParticleSystem->update(duration);
}

void BlobGameSystem::draw()
{
	mParticleSystem->draw();
	if (mDrawDebug)
	{
		drawDebug();
	}
}