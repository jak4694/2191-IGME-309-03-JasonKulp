#pragma once

#include "MyEntityManager.h"

namespace Simplex
{
	class MyOctant
	{
		static uint m_uOctantCount; //will store the number of octants initialized
		static uint m_uMaxLevel; //will store the maximum level an octant can go to
		static uint m_uIdealEntityCount; //will tell how many ideal entities this object will contain

		uint m_uID = 0; //will store the current ID for this octant
		uint m_uLevel = 0; //will store the current level of the octant
		uint m_uChildren = 0; //number of children on the octant

		float m_fSize = 0.0f; //size of the octant

		MeshManager* m_pMeshMngr = nullptr; //mesh manager singleton
		MyEntityManager* m_pEntityMngr = nullptr; //entity manager singleton

		vector3 m_v3Center = vector3(0.0f); //will store the center point of the octant
		vector3 m_v3Min = vector3(0.0f); //will store the minimum vector of the octant
		vector3 m_v3Max = vector3(0.0f); //will store the maximum vector of the octant

		MyOctant* m_pParent = nullptr; //will store the parent of the current octant
		MyOctant* m_pChild[8]; //will store the children of the current octant

		std::vector<uint> m_EntityList; //list of entities under this octant (index in entity manager)

		MyOctant* m_pRoot = nullptr; //root octant
		std::vector<MyOctant*> m_lChild; //list of nodes that contain objects (this will be applied to root only)

	public:
		/*
		USAGE: Constructor, will create an object containing all MagnaEntities instances in the mesh
		manager currently contains
		ARGUMENTS:
		- uint a_nMaxLevel = 2 -> sets the maximum level of subdivision
		- uint nIdealEntityCount = 5 -> sets the ideal level of objects per octant
		OUTPUT: class object
		*/
		MyOctant(uint a_nMaxLevel = 2, uint a_nIdealEntityCount = 5);
		/*
		USAGE: Constructor
		ARGUMENTS:
		- vector3 a_v3Center -> center of the octant in global space
		- float a_fSize -> size of each side of the octant volume
		OUTPUT: class object
		*/
		MyOctant(vector3 a_v3Center, float a_fSize);
		/*
		USAGE: Copy constructor
		ARGUMENTS: class object to copy
		OUTPUT: class object instance
		*/
		MyOctant(MyOctant const& other);
		/*
		USAGE: Copy assignment constructor
		ARGUMENTS: class object to copy
		OUTPUT: ---
		*/
		MyOctant& operator=(MyOctant const& other);
		/*
		USAGE: Destructor
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		~MyOctant();
		/*
		USAGE: Changes object contents for other object's
		ARGUMENTS:
		- MyOctant& other -> object to swap content from
		OUTPUT: ---
		*/
		void Swap(MyOctant& other);
		/*
		USAGE: Gets this octant's size
		ARGUMENTS: ---
		OUTPUT: size of octant
		*/
		float GetSize();
		/*
		USAGE: Gets the center of the octant in global space
		ARGUMENTS: ---
		OUTPUT: Center of the octant in global space
		*/
		vector3 GetCenterGlobal();
		/*
		USAGE: Gets the min corner of the octant in global space
		ARGUMENTS: ---
		OUTPUT: Minimum in global space
		*/
		vector3 GetMinGlobal();
		/*
		USAGE: Gets the max corner of the octant in global space
		ARGUMENTS: ---
		OUTPUT: Maximum in global space
		*/
		vector3 GetMaxGlobal();
		/*
		USAGE: Asks if there is a collision with the entity specified by index from
		the bounding object manager
		ARGUMENTS:
		- int a_URBIndex -> Index of the entity in the entity manager
		OUTPUT: check of the collision
		*/
		bool IsColliding(uint a_uRBIndex);
		/*
		USAGE: Displays the MyOctant volume specified by index including the objects underneath
		ARGUMENTS:
		- uint a_nIndex -> MyOctant to be displayed
		- vector3 a_v3Color = C_YELLOW -> color of the volume to display
		OUTPUT: ---
		*/
		void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);
		/*
		USAGE: Displays the MyOctant volume in the color specified
		ARGUMENTS:
		- vector3 a_v3Color = C_YELLOW -> color of the volume to display
		OUTPUT: ---
		*/
		void Display(vector3 a_v3Color = C_YELLOW);
		/*
		USAGE: Displays the non empty leafs in the octree
		ARGUMENTS:
		- vector3 a_v3Color = C_YELLOW -> color of the volume to display
		OUTPUT: ---
		*/
		void DisplayLeafs(vector3 a_v3Color = C_YELLOW);
		/*
		USAGE: Clears the entity list for each node
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void ClearEntityList();
		/*
		USAGE: allocates 8 smaller octants in the child pointers
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Subdivide();
		/*
		USAGE: returns the child specified in the index
		ARGUMENTS: uint a_nChild -> index of the child (from 0 to 7)
		OUTPUT: MyOctant object (child in index)
		*/
		MyOctant* GetChild(uint a_nChild);
		/*
		USAGE: returns the parent of the octant
		ARGUMENTS: ---
		OUTPUT: MyOctant object (parent)
		*/
		MyOctant* GetParent();
		/*
		USAGE: Asks the MyOctant if it does not contain any children (its a leaf)
		ARGUMENTS: ---
		OUTPUT: It contains no children
		*/
		bool IsLeaf();
		/*
		USAGE: Asks the MyOctant if it contains more than this many bounding objects
		ARGUMENTS:
		- uint a_nEntities -> number of entities to query
		OUTPUT: It contains at least this many entities
		*/
		bool ContainsMoreThan(uint a_nEntities);
		/*
		USAGE: Deletes all children and the children of their children
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void KillBranches();
		/*
		USAGE: Creates a tree using subdivisions, the max number of objects and levels
		ARGUMENTS:
		- uint a_nMaxLevel = 3 -> sets the maximum level of the tree while constructing it
		OUTPUT: ---
		*/
		void ConstructTree(uint a_nMaxLevel = 3);
		/*
		USAGE: Traverse the tree up to leafs and sets the objects in them to the index
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void AssignIDtoEntity();
		/*
		USAGE: Gets the total number of octants in the world
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		uint GetOctantCount();

	private:
		/*
		USAGE: Deallocates member fields
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Release();
		/*
		USAGE: Allocates member fields
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Init();
		/*
		USAGE: Creates the list of all leafs that contain objects
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void ConstructList();
	};
}

