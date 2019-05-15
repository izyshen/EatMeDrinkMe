#pragma once

#include "cs488-framework/BatchInfo.hpp"

#include <glm/glm.hpp>

#include <initializer_list>
#include <vector>
#include <unordered_map>
#include <string>



// String identifier for a mesh.
typedef std::string  MeshId;

// File path to a .obj file.
typedef std::string ObjFilePath;


// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
// object. Each BatchInfo object contains an index offset and the number of indices
// required to render the mesh with identifier MeshId.
typedef std::unordered_map<MeshId, BatchInfo>  BatchInfoMap;

typedef std::unordered_map<MeshId, glm::vec3> MaxCoordMap;
typedef std::unordered_map<MeshId, glm::vec3> MinCoordMap;


/*
* Class for consolidating all vertex data within a list of .obj files.
*/
class MeshConsolidator {
public:
	MeshConsolidator();

	MeshConsolidator(std::initializer_list<ObjFilePath>  objFileList);

	~MeshConsolidator();

	const float * getVertexPositionDataPtr() const;

	const float * getVertexNormalDataPtr() const;

	const float * getVertexUVDataPtr() const;

	size_t getNumVertexPositionBytes() const;

	size_t getNumVertexNormalBytes() const;

	size_t getNumVertexUVBytes() const;

	void getBatchInfoMap(BatchInfoMap & batchInfoMap) const;

	glm::vec3 getMaxCoordInfo(std::string m);

	glm::vec3 getMinCoordInfo(std::string m);


private:
	std::vector<glm::vec3> m_vertexPositionData;
	std::vector<glm::vec3> m_vertexNormalData;
	std::vector<glm::vec2> m_vertexUVData;

	BatchInfoMap m_batchInfoMap;

	MaxCoordMap m_maxCoordMap;
	MinCoordMap m_minCoordMap;

};


