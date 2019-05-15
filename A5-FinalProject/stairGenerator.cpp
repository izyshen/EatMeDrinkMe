// used to generate stairs
#include <iostream>
using namespace std;

int stairGenerator() {
	float dim = 1.2;
	float y = -29.1;
	float z = -23.5;
	for (int i=0; i<20; ++i) {
		cout << "stair" << i << " = gr.mesh('cube', 'stair" << i << "')" << endl; 
		cout << "stair" << i << ":scale(6.0, " << (float)dim << ", 3.0)" << endl;
		cout << "stair" << i << ":translate(-27.0, " << y << ", " << z << ")" << endl;
		cout << "stair" << i << ":set_material(whiteish)" << endl;
		cout << "rootNode:add_child(stair" << i << ")" << endl << endl;
		dim += 1.2;
		y += (dim/2);
		z += 1.6;
	}

// 	stair0 = gr.mesh('cube', 'stair0')
// stair0:scale(6.0, 1.4, 3.0)
// stair0:translate(-27.0, -29.1, -23.5)
// stair0:set_material(whiteish)
// rootNode:add_child(stair0)

// stair1 = gr.mesh('cube', 'stair1')
// stair1:scale(6.0, 3.6, 3.0)
// stair1:translate(-27.0, -28.2, -22.0)
// stair1:set_material(whiteish)
// rootNode:add_child(stair1)

	// for (int i=20; i<40; ++i) {
	// 	cout << "stair" << i << " = gr.mesh('cube', 'stair" << i << "')" << endl; 
	// 	cout << "stair" << i << ":scale(10.0, " << (float)dim << ", 5.0)" << endl;
	// 	cout << "stair" << i << ":translate(-30.0, 10.0, " << (float)dim-5.0 << ".0)" << endl;
	// 	cout << "stair" << i << ":rotate('y', 90)" << endl;
	// 	cout << "stair" << i << ":set_material(whiteish)" << endl;
	// 	cout << "table:add_child(stair" << i << ")" << endl << endl;
	// 	dim += 5.0;
	// }

	return 0;
	
}
