-- Create the top level root node named 'root'.
rootNode = gr.node('root')
-- rootNode:translate(25.0, -5.0, 20)

prettyBlue = gr.material({0.2, 0.6, 0.6}, {0.8, 0.8, 0.8}, 10.0)
red = gr.material({0.8, 0.2, 0.2}, {0.8, 0.8, 0.8}, 10.0)
whiteish = gr.material({0.8, 0.8, 0.8}, {0.8, 0.8, 0.8}, 10.0)
redCup = gr.material({0.76, 0.16, 0.2}, {0.8, 0.8, 0.8}, 10.0)
yellowish = gr.material({0.764, 0.729, 0.552}, {0.8, 0.8, 0.8}, 10.0)
brown = gr.material({0.392, 0.192, 0.168}, {0.8, 0.8, 0.8}, 10.0)
hiddenColour = gr.material({200/255.0, 191/255.0, 231/255.0}, {0.8, 0.8, 0.8}, 10.0)
lowYellow = gr.material({0.831, 0.643, 0.250}, {0.8, 0.8, 0.8}, 10.0)

doorKnob = gr.mesh('doorKnob', 'doorKnob')
doorKnob:set_material(lowYellow)
-- doorKnob:scale(2.0, 2.0, 2.0)
rootNode:add_child(doorKnob)

return rootNode
