-- create the top level root node named 'root'.

rootNode = gr.node('root')
rootNode:translate(0.0, 1.0, -10.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
-- blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
-- green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)

-- create the belly
belly = gr.mesh('sphere', 'belly')
belly:translate(0.0, -1.0, 0.0)
belly:scale(2.0, 2.0, 1.7)
belly:set_material(white)
rootNode:add_child(belly)

-- upper back
upperBack = gr.mesh('sphere', 'upperBack')
upperBack:translate(0.0, 2.0, 0.0)
upperBack:scale(0.6, 0.4, 0.7)
upperBack:set_material(white)
belly:add_child(upperBack)

-- create upper chest
upperChestLeft = gr.mesh('sphere', 'upperChestLeft')
upperChestLeft:translate(-0.5, 0.5, 5.0)
upperChestLeft:scale(0.9, 0.6, 0.2)
upperChestLeft:rotate('z', 60)
-- upperChestLeft:rotate('x', -60)
upperChestLeft:set_material(white)
upperBack:add_child(upperChestLeft)

upperChestRight = gr.mesh('sphere', 'upperChestRight')
upperChestRight:translate(0.5, 0.5, 5.0)
upperChestRight:scale(0.9, 0.6, 0.2)
upperChestRight:rotate('z', -60)
-- upperChestLeft:rotate('x', 60)
upperChestRight:set_material(white)
upperBack:add_child(upperChestRight)

-- create neckJoint
neckJoint = gr.joint('neckJoint', {-1.0, 0, 1.0}, {-1.0, 0, 1.0})
upperBack:add_child(neckJoint)

-- create head
head = gr.mesh('sphere', 'head')
head:translate(0.0, 1.7, 0.3)
-- head:scale(0.68, 0.68, 0.68)
head:scale(1.0, 1.0, 1.0)
head:set_material(white)
neckJoint:add_child(head)

-- create eyes
eyeLeft = gr.mesh('sphere', 'eyeLeft')
eyeLeft:translate(-1.0, 1.0, 4.0)
eyeLeft:scale(0.2, 0.1, 0.2)
eyeLeft:set_material(black)
head:add_child(eyeLeft)
eyeRight = gr.mesh('sphere', 'eyeRight')
eyeRight:translate(1.0, 1.0, 4.0)
eyeRight:scale(0.2, 0.1, 0.2)
eyeRight:set_material(black)
head:add_child(eyeRight)

-- create beard
beardLeft = gr.mesh('sphere', 'beardLeft')
beardLeft:scale(0.5, 1.3, 0.5)
beardLeft:rotate('z', -25)
beardLeft:translate(-1.0, -1.4, 0.6)
beardLeft:set_material(white)
head:add_child(beardLeft)
beardRight = gr.mesh('sphere', 'beardRight')
beardRight:scale(0.5, 1.3, 0.5)
beardRight:rotate('z', 25)
beardRight:translate(1.0, -1.4, 0.6)
beardRight:set_material(white)
head:add_child(beardRight)

-- create shoulder joints
shoulderJointLeft = gr.joint('ShoulderLeft', {-2.1, 0, 2.1}, {-2.1, 0, 2.1})
upperBack:add_child(shoulderJointLeft)
shoulderJointRight = gr.joint('ShoulderRight', {-2.1, 0, 2.1}, {-2.1, 0, 2.1})
upperBack:add_child(shoulderJointRight)

-- create arms
armLeft = gr.mesh('sphere', 'armLeft')
armLeft:scale(1.8, 0.7, 0.7)
armLeft:translate(-1.2, 0.5, 0.0)
armLeft:rotate('z', 50)
armLeft:set_material(white)
shoulderJointLeft:add_child(armLeft)

armRight = gr.mesh('sphere', 'armRight')
-- armRight:scale(0.4, 0.7, 0.5)
-- armRight:translate(1.0, 0.0, 0.0)
-- armRight:rotate('z', 20)
armRight:scale(1.8, 0.7, 0.7)
armRight:translate(1.2, 0.5, 0.0)
armRight:rotate('z', -50)
armRight:set_material(white)
shoulderJointRight:add_child(armRight)

-- create elbows
elbowLeft = gr.joint('elbowLeft', {-0.2, 0.0, 0.2}, {-0.2, 0.0, 0.2})
armLeft:add_child(elbowLeft);
elbowRight = gr.joint('elbowRight', {-0.2, 0.0, 0.2}, {-0.2, 0.0, 0.2})
armRight:add_child(elbowRight);

-- create lowerArms
-- lowerArmLeft = gr.mesh('sphere', 'lowerArmLeft')
-- lowerArmLeft:scale(0.5, 0.5, 0.5)
-- lowerArmLeft:translate(-1.0, 0.8, 0.0)
-- lowerArmLeft:rotate('y', 0)
-- lowerArmLeft:set_material(white)
-- elbowLeft:add_child(lowerArmLeft)


-- create leg joints
legJointLeft = gr.joint('legJointLeft', {-1.0, 0, 1.0}, {-1.0, 0, 1.0})
belly:add_child(legJointLeft)
legJointRight = gr.joint('legJointRight', {-1.0, 0, 1.0}, {-1.0, 0, 1.0})
belly:add_child(legJointRight)

-- create legs
legLeft = gr.mesh('sphere', 'legLeft')
legLeft:scale(0.5, 0.8, 0.5)
legLeft:translate(-0.7, -0.9, -0.5)
legLeft:rotate('z', 15.0)
legLeft:set_material(white)
legJointLeft:add_child(legLeft)

legRight = gr.mesh('sphere', 'legRight')
legRight:scale(0.5, 0.8, 0.5)
legRight:translate(0.7, -0.9, -0.5)
legRight:rotate('z', -15.0)
legRight:set_material(white)
legJointRight:add_child(legRight)

-- create sheet
sheet = gr.mesh('cube', 'sheet')
sheet:scale(0.4, 0.3, 0.1)
sheet:translate(0.0, -0.9, 0.3)
sheet:set_material(red)
belly:add_child(sheet)

-- create hat
hatLower = gr.mesh('sphere', 'hatLower')
hatLower:scale(1.0, 0.3, 1.0)
hatLower:translate(0.0, 1.0, 0.0)
hatLower:set_material(red)
head:add_child(hatLower)
hatUpper = gr.mesh('cube', 'hatUpper')
hatUpper:scale(1.0, 0.7, 1.0)
hatUpper:translate(0.0, 1.0, 0.0)
hatUpper:set_material(red)
hatLower:add_child(hatUpper)

-- tail
tail = gr.mesh('sphere', 'tail')
tail:scale(0.1, 0.4, 0.2)
tail:translate(0.0, -1.1, -0.2)
tail:rotate('x', 60)
tail:set_material(white)
belly:add_child(tail)		


-- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- equal to the return value from this Lua script.
return rootNode
