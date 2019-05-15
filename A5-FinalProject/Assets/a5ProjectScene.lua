-- Create the top level root node named 'root'.
rootNode = gr.node('root')

-------------------- PRESET MATERIALS -----------------------------------
prettyBlue = gr.material({0.2, 0.6, 0.6}, {0.8, 0.8, 0.8}, 10.0)
red = gr.material({0.8, 0.2, 0.2}, {0.8, 0.8, 0.8}, 10.0)
whiteish = gr.material({0.8, 0.8, 0.8}, {0.8, 0.8, 0.8}, 10.0)
redCup = gr.material({0.76, 0.16, 0.2}, {0.8, 0.8, 0.8}, 10.0)
yellowish = gr.material({0.764, 0.729, 0.552}, {0.8, 0.8, 0.8}, 10.0)
brown = gr.material({0.392, 0.192, 0.168}, {0.8, 0.8, 0.8}, 10.0)
hiddenColour = gr.material({200/255.0, 191/255.0, 231/255.0}, {0.8, 0.8, 0.8}, 10.0)
lowYellow = gr.material({0.831, 0.643, 0.250}, {0.8, 0.8, 0.8}, 10.0)
pink = gr.material({0.960, 0.639, 0.921}, {0.8, 0.8, 0.8}, 10.0)


----------------------------- NODES ----------------------------------
room = gr.mesh('room', 'room')
room:scale(60.0, 60.0, 60.0)
room:set_material(red)
rootNode:add_child(room)

table = gr.mesh('table', 'table')
table:scale(0.5, 0.5, 0.5)
table:translate(0.0, 0.0, -30.0)
table:rotate('x', -90.0)
table:set_material(whiteish)
rootNode:add_child(table)

queenPainting = gr.mesh('cube', 'queenOfHeartsPainting')
queenPainting:scale(24.0, 40.0, 0.6)
queenPainting:translate(0.0, 0.0, -29.9)
queenPainting:set_material(prettyBlue)
rootNode:add_child(queenPainting)

cup = gr.mesh('cup', 'cup')
cup:scale(0.3, 0.3, 0.3)
cup:translate(3.0, -20.0, 5.0)
cup:set_material(redCup)
rootNode:add_child(cup)

drinkMe = gr.mesh('cube', 'drinkMe')
drinkMe:scale(0.8, 0.02, 0.8)
drinkMe:translate(1.0, -19.0, 3.0)
drinkMe:set_material(yellowish)
rootNode:add_child(drinkMe)

door = gr.mesh('cube', 'door')
door:scale(5, 10, 1.2)
door:translate(27.6, -25.2, -29.4)
door:set_material(brown)
rootNode:add_child(door)

doorKnob = gr.mesh('doorKnob', 'doorKnob')
doorKnob:scale(0.1, 0.1, 0.1)
doorKnob:translate(-28.5, -25.5, -29.5)
doorKnob:rotate('y', -90)
doorKnob:set_material(lowYellow)
rootNode:add_child(doorKnob)

key = gr.mesh('key', 'key')
key:scale(0.5, 0.5, 0.5)
key:translate(-3.0, -1.0, 25.0)
key:set_material(lowYellow)
rootNode:add_child(key)

painting1 = gr.mesh('cube', 'painting1')
painting1:scale(0.01, 8, 6)
painting1:translate(25.5, 15.0, -20.0)
painting1:set_material(pink)
rootNode:add_child(painting1)

painting2 = gr.mesh('cube', 'painting2')
painting2:scale(0.01, 8, 6)
painting2:translate(25.5, 0.0, 0.0)
painting2:set_material(pink)
rootNode:add_child(painting2)

painting3 = gr.mesh('cube', 'painting3')
painting3:scale(0.01, 8, 6)
painting3:translate(25.5, 7.0, 20.0)
painting3:set_material(pink)
rootNode:add_child(painting3)

case1 = gr.mesh('cube', 'case1')
case1:scale(9, 9, 9)
case1:translate(25.5, 15.0, -20.0)
case1:set_material(pink)
rootNode:add_child(case1)

case2 = gr.mesh('cube', 'case2')
case2:scale(9, 9, 9)
case2:rotate('y', -90)
case2:translate(25.5, 0.0, 0.0)
case2:set_material(pink)
rootNode:add_child(case2)

case3 = gr.mesh('cube', 'case3')
case3:scale(9, 9, 9)
case3:translate(25.5, 7.0, 20.0)
case3:set_material(pink)
rootNode:add_child(case3)



--------------------------- stairs -----------------------------

stair1 = gr.mesh('cube', 'stair1')
stair1:scale(6.0, 1, 3.0)
stair1:translate(-27.0, -29.5, -13.5)
stair1:set_material(whiteish)
rootNode:add_child(stair1)

stair2 = gr.mesh('cube', 'stair2')
stair2:scale(6.0, 3, 3.0)
stair2:translate(-27.0, -28.5, -10.5)
stair2:set_material(whiteish)
rootNode:add_child(stair2)

stair3 = gr.mesh('cube', 'stair3')
stair3:scale(6.0, 5, 3.0)
stair3:translate(-27.0, -27.5, -7.5)
stair3:set_material(whiteish)
rootNode:add_child(stair3)

stair4 = gr.mesh('cube', 'stair4')
stair4:scale(6.0, 7, 3.0)
stair4:translate(-27.0, -26.5, -4.5)
stair4:set_material(whiteish)
rootNode:add_child(stair4)

stair5 = gr.mesh('cube', 'stair5')
stair5:scale(6.0, 9, 3.0)
stair5:translate(-27.0, -25.5, -1.5)
stair5:set_material(whiteish)
rootNode:add_child(stair5)

stair6 = gr.mesh('cube', 'stair6')
stair6:scale(6.0, 11, 3.0)
stair6:translate(-27.0, -24.5, 1.5)
stair6:set_material(whiteish)
rootNode:add_child(stair6)

stair7 = gr.mesh('cube', 'stair7')
stair7:scale(6.0, 13, 3.0)
stair7:translate(-27.0, -23.5, 4.5)
stair7:set_material(whiteish)
rootNode:add_child(stair7)

stair8 = gr.mesh('cube', 'stair8')
stair8:scale(6.0, 15, 3.0)
stair8:translate(-27.0, -22.5, 7.5)
stair8:set_material(whiteish)
rootNode:add_child(stair8)

stair9 = gr.mesh('cube', 'stair9')
stair9:scale(6.0, 17, 3.0)
stair9:translate(-27.0, -21.5, 10.5)
stair9:set_material(whiteish)
rootNode:add_child(stair9)

stair10 = gr.mesh('cube', 'stair10')
stair10:scale(6.0, 19, 3.0)
stair10:translate(-27.0, -20.5, 13.5)
stair10:set_material(whiteish)
rootNode:add_child(stair10)

stair11 = gr.mesh('cube', 'stair11')
stair11:scale(6.0, 21, 3.0)
stair11:translate(-27.0, -19.5, 16.5)
stair11:set_material(whiteish)
rootNode:add_child(stair11)

stair12 = gr.mesh('cube', 'stair12')
stair12:scale(6.0, 23, 3.0)
stair12:translate(-27.0, -18.5, 19.5)
stair12:set_material(whiteish)
rootNode:add_child(stair12)

stair13 = gr.mesh('cube', 'stair13')
stair13:scale(6.0, 25, 3.0)
stair13:translate(-27.0, -17.5, 22.5)
stair13:set_material(whiteish)
rootNode:add_child(stair13)

stair14 = gr.mesh('cube', 'stair14')
stair14:scale(6.0, 27.0, 3.0)
stair14:translate(-27.0, -16.5, 25.5)
stair14:set_material(whiteish)
rootNode:add_child(stair14)

stair15 = gr.mesh('cube', 'stair15')
stair15:scale(6.0, 29.0, 3.0)
stair15:translate(-27.0, -15.5, 28.5)
stair15:set_material(whiteish)
rootNode:add_child(stair15)

platform = gr.mesh('cube', 'platform')
platform:scale(42, 1.2, 15)
platform:translate(-5.0, -2, 30)
platform:set_material(whiteish)
rootNode:add_child(platform)

return rootNode
