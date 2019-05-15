-- Create the top level root node named 'root'.
rootNode = gr.node('root')
-- rootNode:translate(0.0, 0.0, -5.0)

cubeMesh = gr.mesh('cube', 'name-of-cube')
cubeMesh:scale(1.0, 1.0, 1.0)
cubeMesh:rotate('y', 45.0)
cubeMesh:translate(0.0, 0.0, 0.0)
cubeMesh:set_material(gr.material({0.8, 0.2, 0.2}, {0.8, 0.8, 0.8}, 10.0))
rootNode:add_child(cubeMesh)

return rootNode
