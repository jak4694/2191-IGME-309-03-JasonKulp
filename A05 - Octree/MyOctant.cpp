#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;

//Gets the octant count
uint MyOctant::GetOctantCount()
{
	return m_uOctantCount;
}

//Gets the parent of the octant
MyOctant* MyOctant::GetParent()
{
	return m_pParent;
}

//Gets the size of the ocant
float MyOctant::GetSize()
{
	return m_fSize;
}

//Gets the center global vector3
vector3 MyOctant::GetCenterGlobal()
{
	return m_v3Center;
}

//Gets the min global vector3
vector3 MyOctant::GetMinGlobal()
{
	return m_v3Min;
}

//Gets the max global vector3
vector3 MyOctant::GetMaxGlobal()
{
	return m_v3Max;
}

//Gets the child octant at a given index
MyOctant* MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild > 7)
	{
		return nullptr;
	}
	return m_pChild[a_nChild];
}

//Constructor for the octant using a max level and ideal entity count
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	//Initializes the octant and sets up the parameters
	Init();
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;
	m_pRoot = this;
	m_lChild.clear();
	//Makes a rigid body using the bounding points of the entities
	std::vector<vector3> boundingPoints;
	int entityCount = m_pEntityMngr->GetEntityCount();
	for(int i = 0; i < entityCount; i++)
	{
		MyEntity* entity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* rigidBody = entity->GetRigidBody();
		boundingPoints.push_back(rigidBody->GetMinGlobal());
		boundingPoints.push_back(rigidBody->GetMaxGlobal());
	}
	MyRigidBody* rigidBody = new MyRigidBody(boundingPoints);
	//Calculates the maximum size using the rigid body
	vector3 halfWidth = rigidBody->GetHalfWidth();
	float maxSize = halfWidth.x;
	if(halfWidth.y > maxSize)
	{
		maxSize = halfWidth.y;
	}
	if(halfWidth.z > maxSize)
	{
		maxSize = halfWidth.z;
	}
	//Sets the calculated values, constructs the tree, and destroys the rigid body
	m_v3Center = rigidBody->GetCenterGlobal();
	m_v3Min = m_v3Center - vector3(maxSize);
	m_v3Max = m_v3Center + vector3(maxSize);
	m_fSize = maxSize * 2.0f;
	m_uOctantCount++;
	ConstructTree(m_uMaxLevel);
	SafeDelete(rigidBody);
}

//Constructor for the octant using a center and a size
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
	m_v3Min = m_v3Center - (vector3(a_fSize) / 2);
	m_v3Max = m_v3Center + (vector3(a_fSize) / 2);
	m_uOctantCount++;
}

//Copy constructor for the octant
MyOctant::MyOctant(MyOctant const& other)
{
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;
	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	for(int i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}

//Copy assignment constructor for the octant
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyOctant octant(other);
		Swap(octant);
	}
	return *this;
}

//Destructor for the octant
MyOctant::~MyOctant()
{
	Release();
}

//Swaps the values of the octant with the values of the one passed in
void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);
	for (int i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}

//Checks if the octant is colliding with a given entity
bool MyOctant::IsColliding(uint a_uRBIndex)
{
	//Accesses the entity
	int entityCount = m_pEntityMngr->GetEntityCount();
	if(a_uRBIndex >= entityCount)
	{
		return false;
	}
	MyEntity* entity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* rigidBody = entity->GetRigidBody();
	vector3 minOther = rigidBody->GetMinGlobal();
	vector3 maxOther = rigidBody->GetMaxGlobal();
	//Does AABB checks on the octant and the entity
	if(m_v3Max.x < minOther.x)
	{
		return false;
	}
	if(m_v3Min.x > maxOther.x)
	{
		return false;
	}
	if(m_v3Max.y < minOther.y)
	{
		return false;
	}
	if(m_v3Min.y > maxOther.y)
	{
		return false;
	}
	if(m_v3Max.z < minOther.z)
	{
		return false;
	}
	if(m_v3Min.z > maxOther.z)
	{
		return false;
	}
	return true;
}

//Displays the octant with the given index and any children
void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if(m_uID == a_nIndex)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}
	for(int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_nIndex);
	}
}

//Displays the octant and any children
void MyOctant::Display(vector3 a_v3Color)
{
	for(int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

//Displays the leafs of the octant
void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	int leafCount = m_lChild.size();
	for(int i = 0; i < leafCount; i++)
	{
		m_lChild[i]->DisplayLeafs(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

//Clears the entity list
void MyOctant::ClearEntityList()
{
	for(int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ClearEntityList();
	}
	m_EntityList.clear();
}

//Subdivides the octant
void MyOctant::Subdivide()
{
	//Ignore if it has reached the max levels
	if(m_uLevel >= m_uMaxLevel)
	{
		return;
	}
	//Ignore if it already has children
	if(m_uChildren != 0)
	{
		return;
	}
	//Set the children count and its size
	m_uChildren = 8;
	float size = m_fSize / 4.0f;
	float doubleSize = size * 2.0f;
	//Create each child octant
	vector3 center;
	center = m_v3Center + vector3(size, size, size);
	m_pChild[0] = new MyOctant(center, doubleSize);
	center = m_v3Center + vector3(size, size, -size);
	m_pChild[1] = new MyOctant(center, doubleSize);
	center = m_v3Center + vector3(size, -size, size);
	m_pChild[2] = new MyOctant(center, doubleSize);
	center = m_v3Center + vector3(size, -size, -size);
	m_pChild[3] = new MyOctant(center, doubleSize);
	center = m_v3Center + vector3(-size, size, size);
	m_pChild[4] = new MyOctant(center, doubleSize);
	center = m_v3Center + vector3(-size, size, -size);
	m_pChild[5] = new MyOctant(center, doubleSize);
	center = m_v3Center + vector3(-size, -size, size);
	m_pChild[6] = new MyOctant(center, doubleSize);
	center = m_v3Center + vector3(-size, -size, -size);
	m_pChild[7] = new MyOctant(center, doubleSize);
	//Set the root, parent, and level of each child octant
	for(int i = 0; i < 8; i++)
	{
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		//Subdivide the child again if it hase more than the ideal entity count
		if(m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[i]->Subdivide();
		}
	}
}

//Check if the octant is a leaf
bool MyOctant::IsLeaf()
{
	return m_uChildren == 0;
}

//Check if the octant contains more than a given number of entities
bool MyOctant::ContainsMoreThan(uint a_nEntities)
{
	int entityCount = m_pEntityMngr->GetEntityCount();
	int count = 0;
	for(int i = 0; i < entityCount; i++)
	{
		if(IsColliding(i))
		{
			count++;
		}
		if(count > a_nEntities)
		{
			return true;
		}
	}
	return false;
}

//Recursively kills the children and any sub-children
void MyOctant::KillBranches()
{
	for(int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->KillBranches();
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}
	m_uChildren = 0;
}

//Creates the octant tree
void MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//Ignore if the level is not zero
	if(m_uLevel != 0)
	{
		return;
	}
	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;
	ClearEntityList();
	KillBranches();
	m_lChild.clear();
	if(ContainsMoreThan(m_uIdealEntityCount))
	{
		Subdivide();
	}
	AssignIDtoEntity();
	ConstructList();
}

//Assign IDs to all the entities
void MyOctant::AssignIDtoEntity()
{
	for(int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}
	if(m_uChildren == 0)
	{
		int entityCount = m_pEntityMngr->GetEntityCount();
		for(int i = 0; i < entityCount; i++)
		{
			if(IsColliding(i))
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}

//Release the octant (used in the destructor)
void MyOctant::Release()
{
	if(m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}

//Initialize the octant with default values
void MyOctant::Init()
{
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}

//Creates the list of objects
void MyOctant::ConstructList()
{
	for(int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ConstructList();
	}
	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}
