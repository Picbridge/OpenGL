# Spatial_partitioning

A hybrid rendering pipeline that employs a **Deferred Rendering** , **bounding volume hierarchy**, and **Adaptive Octree and BSP-tree representation**.

## Features

### Controls

  - Use W,A,S,D,Q,E to move the camera.

### Lightsource
  
  - Generate/Deletes random colored light
  
  ![Light](https://github.com/Picbridge/Spatial_partitioning/blob/main/gif/light.gif)
  
### Bounding Volumes

  - AABB, OBB Bounding Sphere, Bounding Sphere(centroid), Larsson's method, Ritter’s method, Bounding Sphere (PCA method)
  
  ![BoundingVolumes](https://github.com/Picbridge/Spatial_partitioning/blob/main/gif/BoundingVolume.gif)
  
  - Top-down constructionAABB,Bounding Spheres
  
  - Bottom-up construction Larsson's method

  ![Hierarchy](https://github.com/Picbridge/Spatial_partitioning/blob/main/gif/BV_Hierarchy.gif)

### Spatial Partitioning
  
  -Octree: Adaptive octree in top-down fashion. Shows all levels of the octree in different colors
  
  -BST Tree: Shows all leaf nodes of the BSP tree in a different color
  
  ![Partitioning](https://github.com/Picbridge/Spatial_partitioning/blob/main/gif/Partition.gif)
