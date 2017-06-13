//Steve Trettel's Hypercube Projection
//
// Dennis Adderton
// Summer 2017 revision 01
// stereoprojection01
/*



*/

// Draw the space
// Using AlloSystem Omni App named "doApp"

#include "al_OmniApp.hpp"
#include "allocore/io/al_App.hpp"
#include "allocore/graphics/al_Font.hpp"
#include <iostream>
using namespace al;
using namespace std;
#define N 900 // number of data points

struct doApp : OmniApp {

	Material material;
	Light light;
	Font font1;
	Mesh ball;
  
  	Vec3f position[N*8];
  	Vec3f radius[N*8];
  	Vec3f color[N*8];
	Vec4f hypercube_vertices[16];

	float a[N]; // "x"
	float b[N]; // "y"
	float c[N]; // "z"
	float d[N]; // "w"
	float e[N]; // "t"
	float p1; float p2; float p3; float p4;
	float q1; float q2; float q3; float q4;

	float root3;
	float phi; //1.618
	float phi_inv; //.618
	float rotation;
	float tri_scale;
	float sphere_scale;
	float plato_scale;

	float gain;
	float zoom;
	float focus_a; // focus, changed from marker_, could add a cursor
	float focus_b;
	float focus_c;
	float focus_d;
	float focus_e; // can be used for time
	float time_scale;
	float time_shift;
	float length; //depth of trigraph
	float gap; //spacing for crosshairs, should be come cursor

	int focus; //same as focus_t, could become cursor
	int datasize; //same as N*8

	bool  axes_XYZ;
	bool  axes_wxyz;
	bool  axes_3D;
	bool  axes_4D;
	bool  axes_ratios;
	bool  trigraph_4D;
	bool  surface;	
	bool  lines;	
	bool  time_data;
	bool  mean_data;
	bool  time_axis; // why doesn't this toggle like a bool?
	bool  tetrahedron1;
	bool  tetrahedron2;
	bool  cube;
	bool  octohedron;
	bool  dodecahedron;
	bool  icosahedron;

// Make pointmesh and then make 3-vector.
// Create 32 meshes for lines drawn to indicate edges of hypercube.   
	Mesh lineMeshQ1Z; std::vector<Vec3f> edgeLineQ1Z;
	Mesh lineMeshQ1Y; std::vector<Vec3f> edgeLineQ1Y;
	Mesh lineMeshQ1X; std::vector<Vec3f> edgeLineQ1X;
	Mesh lineMeshQ1W; std::vector<Vec3f> edgeLineQ1W;

	Mesh lineMeshQ2Z; std::vector<Vec3f> edgeLineQ2Z;
	Mesh lineMeshQ2Y; std::vector<Vec3f> edgeLineQ2Y;
	Mesh lineMeshQ2X; std::vector<Vec3f> edgeLineQ2X;	
	Mesh lineMeshQ2W; std::vector<Vec3f> edgeLineQ2W;

	Mesh lineMeshQ3Z; std::vector<Vec3f> edgeLineQ3Z;
	Mesh lineMeshQ3Y; std::vector<Vec3f> edgeLineQ3Y;
	Mesh lineMeshQ3X; std::vector<Vec3f> edgeLineQ3X;	
	Mesh lineMeshQ3W; std::vector<Vec3f> edgeLineQ3W;
	
	Mesh lineMeshQ4Z; std::vector<Vec3f> edgeLineQ4Z;
	Mesh lineMeshQ4Y; std::vector<Vec3f> edgeLineQ4Y;
	Mesh lineMeshQ4X; std::vector<Vec3f> edgeLineQ4X;	
	Mesh lineMeshQ4W; std::vector<Vec3f> edgeLineQ4W;
	
	Mesh lineMeshQ5Z; std::vector<Vec3f> edgeLineQ5Z;
	Mesh lineMeshQ5Y; std::vector<Vec3f> edgeLineQ5Y;
	Mesh lineMeshQ5X; std::vector<Vec3f> edgeLineQ5X;
	Mesh lineMeshQ5W; std::vector<Vec3f> edgeLineQ5W;
	
	Mesh lineMeshQ6Z; std::vector<Vec3f> edgeLineQ6Z;
	Mesh lineMeshQ6Y; std::vector<Vec3f> edgeLineQ6Y;
	Mesh lineMeshQ6X; std::vector<Vec3f> edgeLineQ6X;	
	Mesh lineMeshQ6W; std::vector<Vec3f> edgeLineQ6W;
	
	Mesh lineMeshQ7Z; std::vector<Vec3f> edgeLineQ7Z;
	Mesh lineMeshQ7Y; std::vector<Vec3f> edgeLineQ7Y;
	Mesh lineMeshQ7X; std::vector<Vec3f> edgeLineQ7X;	
	Mesh lineMeshQ7W; std::vector<Vec3f> edgeLineQ7W;
	
	Mesh lineMeshQ8Z; std::vector<Vec3f> edgeLineQ8Z;
	Mesh lineMeshQ8Y; std::vector<Vec3f> edgeLineQ8Y;
	Mesh lineMeshQ8X; std::vector<Vec3f> edgeLineQ8X;	
	Mesh lineMeshQ8W; std::vector<Vec3f> edgeLineQ8W;

// CONSTRUCTOR
doApp() :font1("./AlloSystem/allocore/share/fonts/VeraMoIt.ttf", 20) {
//doApp() :font1("./AlloSystem/allocore/share/fonts/VeraMoIt.ttf", 20) {

	tetrahedron1 = false;
	tetrahedron2 = false;
	cube = false;
	octohedron = false;
	dodecahedron = false;
	icosahedron = false;
	axes_XYZ = false;
	axes_wxyz = false;
	axes_3D = false;
	axes_4D = false;
	axes_ratios = false;
	trigraph_4D = false;
	surface = false;
	lines = true;
	time_data = false;
	mean_data = false;
	time_axis = false;
		
	root3 = 1.732;
	sphere_scale = root3;
	tri_scale = root3;
	plato_scale = 3.0;
	phi = 1.618;
	phi_inv = 0.618;
	rotation = 0;
	focus_e = 1.0;

	gain = 10;
	zoom = 1;
	focus_a = 1;
	focus_b = 1;
	focus_c = 1;
	focus_d = 1;
	time_scale = 0.3536;
	time_shift = 1.0;
		
	length = 0.0;//0.732;
	gap = 0.0;//-0.732;
	focus = 1.0; 
	datasize = 1.0;

	nav().pos(0.0, 0.0, 0.0);
	light.pos(0, 10.0, 0);
	light.specular(Color(0,0,0));
	light.diffuse(Color(0,0,0));
	light.ambient(Color(1,1,1));
	initWindow();
	initAudio();
      	
    // list of vertices for hypercube
    hypercube_vertices[0]  = Vec4f(-1.0,-1.0,-1.0,-1.0); // 
    hypercube_vertices[1]  = Vec4f(-1.0,-1.0,-1.0, 1.0); // 
    hypercube_vertices[2]  = Vec4f(-1.0,-1.0, 1.0,-1.0); // 
    hypercube_vertices[3]  = Vec4f(-1.0,-1.0, 1.0, 1.0); // 
    hypercube_vertices[4]  = Vec4f(-1.0, 1.0,-1.0,-1.0); // 
    hypercube_vertices[5]  = Vec4f(-1.0, 1.0,-1.0, 1.0); // 
    hypercube_vertices[6]  = Vec4f(-1.0, 1.0, 1.0,-1.0); // 
    hypercube_vertices[7]  = Vec4f(-1.0, 1.0, 1.0, 1.0); // 
    hypercube_vertices[8]  = Vec4f( 1.0,-1.0,-1.0,-1.0); // 
    hypercube_vertices[9]  = Vec4f( 1.0,-1.0,-1.0, 1.0); // 
    hypercube_vertices[10] = Vec4f( 1.0,-1.0, 1.0,-1.0); // 
    hypercube_vertices[11] = Vec4f( 1.0,-1.0, 1.0, 1.0); //
    hypercube_vertices[12] = Vec4f( 1.0, 1.0,-1.0,-1.0); // 
    hypercube_vertices[13] = Vec4f( 1.0, 1.0,-1.0, 1.0); // 
    hypercube_vertices[14] = Vec4f( 1.0, 1.0, 1.0,-1.0); // 
    hypercube_vertices[15] = Vec4f( 1.0, 1.0, 1.0, 1.0); // 

    // Vertex pairs to make hypercube edges
    p1 = -0.5; p2 = -0.5; p3 = -0.5; p4 = -0.5;
    q1 = -0.5; q2 = -0.5; q3 = -0.5; q4 =  0.5;
    // Meshes
	// Balls
	ball.primitive(Graphics::TRIANGLES);
	addSphere(ball);
	ball.scale(0.01);
	ball.generateNormals();	  

	// Lines
	edgeLineQ1Z.resize(N); edgeLineQ1Y.resize(N); edgeLineQ1X.resize(N); edgeLineQ1W.resize(N);
	edgeLineQ2Z.resize(N); edgeLineQ2Y.resize(N); edgeLineQ2X.resize(N); edgeLineQ2W.resize(N);
	edgeLineQ3Z.resize(N); edgeLineQ3Y.resize(N); edgeLineQ3X.resize(N); edgeLineQ3W.resize(N);
	edgeLineQ4Z.resize(N); edgeLineQ4Y.resize(N); edgeLineQ4X.resize(N); edgeLineQ4W.resize(N);
	edgeLineQ5Z.resize(N); edgeLineQ5Y.resize(N); edgeLineQ5X.resize(N); edgeLineQ5W.resize(N);
	edgeLineQ6Z.resize(N); edgeLineQ6Y.resize(N); edgeLineQ6X.resize(N); edgeLineQ6W.resize(N);
	edgeLineQ7Z.resize(N); edgeLineQ7Y.resize(N); edgeLineQ7X.resize(N); edgeLineQ7W.resize(N);
	edgeLineQ8Z.resize(N); edgeLineQ8Y.resize(N); edgeLineQ8X.resize(N); edgeLineQ8W.resize(N);

		int n = 0;
		for (unsigned row = 1; row < 100; ++row) {
		//for (unsigned col = 0; col < 30; ++col) {
			  
		//float col_pos = (float)col / 10.0 + 0.0;
		//float row_pos = (float)row / 10.0 + 0.538;
		float t = (float)row / 100 ;//col_pos + (row_pos / 30);
			  
	
	// edgeBOLIC LINES
//  {{-(1/2), -(1/2), -(1/2), -(1/2)}, {-(1/2), -(1/2), -(1/2), 1/2}}
    p1 = -0.5; p2 = -0.5; p3 = -0.5; p4 = -0.5;
    q1 = -0.5; q2 = -0.5; q3 = -0.5; q4 =  0.5;
		edgeLineQ1Z[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);								
		lineMeshQ1Z.vertex(edgeLineQ1Z[n]);
		lineMeshQ1Z.color(Color(0.3, 1.0, 0.3, 1.0)); // LIME

//  {{-(1/2), -(1/2), -(1/2), -(1/2)}, {-(1/2), -(1/2), 1/2, -(1/2)}}
    p1 = -0.5; p2 = -0.5; p3 = -0.5; p4 = -0.5;
    q1 = -0.5; q2 = -0.5; q3 =  0.5; q4 = -0.5;
		edgeLineQ1Y[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ1Y.vertex(edgeLineQ1Y[n]);
		lineMeshQ1Y.color(Color(1.0, 0.3, 0.3, 1.0)); // SALMON

//  {{-(1/2), -(1/2), -(1/2), -(1/2)}, {-(1/2), 1/2, -(1/2), -(1/2)}}
    p1 = -0.5; p2 = -0.5; p3 = -0.5; p4 = -0.5;
    q1 = -0.5; q2 =  0.5; q3 = -0.5; q4 = -0.5;
		edgeLineQ1X[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ1X.vertex(edgeLineQ1X[n]);
		lineMeshQ1X.color(Color(1.0, 1.0, 0.0, 1.0)); // YELLOW

//  {{-(1/2), -(1/2), -(1/2), -(1/2)}, {1/2, -(1/2), -(1/2), -(1/2)}}
    p1 = -0.5; p2 = -0.5; p3 = -0.5; p4 = -0.5;
    q1 =  0.5; q2 = -0.5; q3 = -0.5; q4 = -0.5;
		edgeLineQ1W[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);										
		lineMeshQ1W.vertex(edgeLineQ1W[n]);
		lineMeshQ1W.color(Color(0.0, 1.0, 1.0, 1.0)); // CYAN


//  {{-(1/2), -(1/2), -(1/2), 1/2}, {-(1/2), -(1/2), 1/2, 1/2}}
    p1 = -0.5; p2 = -0.5; p3 = -0.5; p4 =  0.5;
    q1 = -0.5; q2 = -0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ2Z[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);										
		lineMeshQ2Z.vertex(edgeLineQ2Z[n]);
		lineMeshQ2Z.color(Color(0.3, 0.3, 1.0, 1.0)); // VIOLET
			  
//  {{-(1/2), -(1/2), -(1/2), 1/2}, {-(1/2), 1/2, -(1/2), 1/2}}
    p1 = -0.5; p2 = -0.5; p3 = -0.5; p4 =  0.5;
    q1 = -0.5; q2 =  0.5; q3 = -0.5; q4 =  0.5;
		edgeLineQ2Y[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ2Y.vertex(edgeLineQ2Y[n]);
		lineMeshQ2Y.color(Color(1.0, 0.0, 1.0, 1.0)); // MAGENTA
			  
// {{-(1/2), -(1/2), -(1/2), 1/2}, {1/2, -(1/2), -(1/2), 1/2}}
    p1 = -0.5; p2 = -0.5; p3 = -0.5; p4 =  0.5;
    q1 =  0.5; q2 = -0.5; q3 = -0.5; q4 =  0.5;
		edgeLineQ2X[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);										
		lineMeshQ2X.vertex(edgeLineQ2X[n]);
		lineMeshQ2X.color(Color(1.0, 1.0, 0.0, 1.0)); // YELLOW

//  {{-(1/2), -(1/2), 1/2, -(1/2)}, {-(1/2), -(1/2), 1/2, 1/2}}
    p1 = -0.5; p2 = -0.5; p3 =  0.5; p4 = -0.5;
    q1 = -0.5; q2 = -0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ2W[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ2W.vertex(edgeLineQ2W[n]);
		lineMeshQ2W.color(Color(0.3, 1.0, 0.3, 1.0)); // LIME

			  
//  {{-(1/2), -(1/2), 1/2, -(1/2)}, {-(1/2), 1/2, 1/2, -(1/2)}}
    p1 = -0.5; p2 = -0.5; p3 =  0.5; p4 = -0.5;
    q1 = -0.5; q2 =  0.5; q3 =  0.5; q4 = -0.5;
		edgeLineQ3Z[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ3Z.vertex(edgeLineQ3Z[n]);
		lineMeshQ3Z.color(Color(0.3, 0.3, 1.0, 1.0)); // VIOLET
			  
//  {{-(1/2), -(1/2), 1/2, -(1/2)}, {1/2, -(1/2), 1/2, -(1/2)}}
    p1 = -0.5; p2 = -0.5; p3 =  0.5; p4 = -0.5;
    q1 =  0.5; q2 = -0.5; q3 =  0.5; q4 = -0.5;
		edgeLineQ3Y[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);							
		lineMeshQ3Y.vertex(edgeLineQ3Y[n]);
		lineMeshQ3Y.color(Color(1.0, 0.3, 0.3, 1.0)); // SALMON
			  
//  {{-(1/2), -(1/2), 1/2, 1/2}, {-(1/2), 1/2, 1/2, 1/2}}
    p1 = -0.5; p2 = -0.5; p3 =  0.5; p4 =  0.5;
    q1 = -0.5; q2 =  0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ3X[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);						
		lineMeshQ3X.vertex(edgeLineQ3X[n]);
		lineMeshQ3X.color(Color(0.0, 1.0, 1.0, 1.0)); // CYAN
			  
//  {{-(1/2), -(1/2), 1/2, 1/2}, {1/2, -(1/2), 1/2, 1/2}}
    p1 = -0.5; p2 = -0.5; p3 =  0.5; p4 =  0.5;
    q1 =  0.5; q2 = -0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ3W[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);										
		lineMeshQ3W.vertex(edgeLineQ3W[n]);
		lineMeshQ3W.color(Color(1.0, 0.0, 1.0, 1.0)); // MAGENTA

		
//  {{-(1/2), 1/2, -(1/2), -(1/2)}, {-(1/2), 1/2, -(1/2), 1/2}}
    p1 = -0.5; p2 =  0.5; p3 = -0.5; p4 = -0.5;
    q1 = -0.5; q2 =  0.5; q3 = -0.5; q4 =  0.5;
		edgeLineQ4Z[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);							
		lineMeshQ4Z.vertex(edgeLineQ4Z[n]);
		lineMeshQ4Z.color(Color(0.3, 1.0, 0.3, 1.0)); // LIME
			  
//  {{-(1/2), 1/2, -(1/2), -(1/2)}, {-(1/2), 1/2, 1/2, -(1/2)}}
    p1 = -0.5; p2 =  0.5; p3 = -0.5; p4 = -0.5;
    q1 = -0.5; q2 =  0.5; q3 =  0.5; q4 = -0.5;
		edgeLineQ4Y[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ4Y.vertex(edgeLineQ4Y[n]);
		lineMeshQ4Y.color(Color(1.0, 0.0, 1.0, 1.0)); // MAGENTA
			  
//  {{-(1/2), 1/2, -(1/2), -(1/2)}, {1/2, 1/2, -(1/2), -(1/2)}}
    p1 = -0.5; p2 =  0.5; p3 = -0.5; p4 = -0.5;
    q1 =  0.5; q2 =  0.5; q3 = -0.5; q4 = -0.5;
		edgeLineQ4X[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ4X.vertex(edgeLineQ4X[n]);
		lineMeshQ4X.color(Color(0.0, 1.0, 1.0, 1.0)); // CYAN
			  
//  {{-(1/2), 1/2, -(1/2), 1/2}, {-(1/2), 1/2, 1/2, 1/2}}
    p1 = -0.5; p2 =  0.5; p3 = -0.5; p4 =  0.5;
    q1 = -0.5; q2 =  0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ4W[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);								
		lineMeshQ4W.vertex(edgeLineQ4W[n]);
		lineMeshQ4W.color(Color(1.0, 0.3, 0.3, 1.0)); // SALMON


//  {{-(1/2), 1/2, -(1/2), 1/2}, {1/2, 1/2, -(1/2), 1/2}}
    p1 = -0.5; p2 =  0.5; p3 = -0.5; p4 =  0.5;
    q1 =  0.5; q2 =  0.5; q3 = -0.5; q4 =  0.5;
		edgeLineQ5Z[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);								
		lineMeshQ5Z.vertex(edgeLineQ5Z[n]);
		lineMeshQ5Z.color(Color(0.3, 0.3, 1.0, 1.0)); // VIOLET
			  
//  {{-(1/2), 1/2, 1/2, -(1/2)}, {-(1/2), 1/2, 1/2, 1/2}}
    p1 = -0.5; p2 =  0.5; p3 =  0.5; p4 = -0.5;
    q1 = -0.5; q2 =  0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ5Y[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);							
		lineMeshQ5Y.vertex(edgeLineQ5Y[n]);
		lineMeshQ5Y.color(Color(1.0, 0.3, 0.3, 1.0)); // SALMON
			  
//  {{-(1/2), 1/2, 1/2, -(1/2)}, {1/2, 1/2, 1/2, -(1/2)}}
    p1 = -0.5; p2 =  0.5; p3 =  0.5; p4 = -0.5;
    q1 =  0.5; q2 =  0.5; q3 =  0.5; q4 = -0.5;
		edgeLineQ5X[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);								
		lineMeshQ5X.vertex(edgeLineQ5X[n]);
		lineMeshQ5X.color(Color(0.0, 1.0, 1.0, 1.0)); // CYAN
		
//  {{-(1/2), 1/2, 1/2, 1/2}, {1/2, 1/2, 1/2, 1/2}}
    p1 = -0.5; p2 =  0.5; p3 =  0.5; p4 =  0.5;
    q1 =  0.5; q2 =  0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ5W[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);								
		lineMeshQ5W.vertex(edgeLineQ5W[n]);
		lineMeshQ5W.color(Color(1.0, 1.0, 0.0, 1.0)); // YELLOW


//  {{1/2, -(1/2), -(1/2), -(1/2)}, {1/2, -(1/2), -(1/2), 1/2}}
    p1 =  0.5; p2 = -0.5; p3 = -0.5; p4 = -0.5;
    q1 =  0.5; q2 = -0.5; q3 = -0.5; q4 =  0.5;
		edgeLineQ6Z[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ6Z.vertex(edgeLineQ6Z[n]);
		lineMeshQ6Z.color(Color(0.3, 1.0, 0.3, 1.0)); // LIME
			  
//  {{1/2, -(1/2), -(1/2), -(1/2)}, {1/2, -(1/2), 1/2, -(1/2)}}
    p1 =  0.5; p2 = -0.5; p3 = -0.5; p4 = -0.5;
    q1 =  0.5; q2 = -0.5; q3 =  0.5; q4 = -0.5;
		edgeLineQ6Y[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ6Y.vertex(edgeLineQ6Y[n]);
		lineMeshQ6Y.color(Color(1.0, 0.0, 1.0, 1.0)); // MAGENTA
			  
//  {{1/2, -(1/2), -(1/2), -(1/2)}, {1/2, 1/2, -(1/2), -(1/2)}}
    p1 =  0.5; p2 = -0.5; p3 = -0.5; p4 = -0.5;
    q1 =  0.5; q2 =  0.5; q3 = -0.5; q4 = -0.5;
		edgeLineQ6X[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ6X.vertex(edgeLineQ6X[n]);
		lineMeshQ6X.color(Color(0.0, 1.0, 1.0, 1.0)); // CYAN
			  
//  {{1/2, -(1/2), -(1/2), 1/2}, {1/2, -(1/2), 1/2, 1/2}}
    p1 =  0.5; p2 = -0.5; p3 = -0.5; p4 =  0.5;
    q1 =  0.5; q2 = -0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ6W[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ6W.vertex(edgeLineQ6W[n]);
		lineMeshQ6W.color(Color(0.3, 0.3, 1.0, 1.0)); // VIOLET
	

//  {{1/2, -(1/2), -(1/2), 1/2}, {1/2, 1/2, -(1/2), 1/2}}
    p1 =  0.5; p2 = -0.5; p3 = -0.5; p4 =  0.5;
    q1 =  0.5; q2 =  0.5; q3 = -0.5; q4 =  0.5;
		edgeLineQ7Z[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);									
		lineMeshQ7Z.vertex(edgeLineQ7Z[n]);
		lineMeshQ7Z.color(Color(0.3, 1.0, 0.3, 1.0)); // LIME
			  
//  {{1/2, -(1/2), 1/2, -(1/2)}, {1/2, -(1/2), 1/2, 1/2}}
    p1 =  0.5; p2 = -0.5; p3 =  0.5; p4 = -0.5;
    q1 =  0.5; q2 = -0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ7Y[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);										
		lineMeshQ7Y.vertex(edgeLineQ7Y[n]);
		lineMeshQ7Y.color(Color(1.0, 0.3, 0.3, 1.0)); // SALMON
			  
//  {{1/2, -(1/2), 1/2, -(1/2)}, {1/2, 1/2, 1/2, -(1/2)}}
    p1 =  0.5; p2 = -0.5; p3 =  0.5; p4 = -0.5;
    q1 =  0.5; q2 =  0.5; q3 =  0.5; q4 = -0.5;
		edgeLineQ7X[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);										
		lineMeshQ7X.vertex(edgeLineQ7X[n]);
		lineMeshQ7X.color(Color(1.0, 1.0, 0.0, 1.0)); // YELLOW
			  
//  {{1/2, -(1/2), 1/2, 1/2}, {1/2, 1/2, 1/2, 1/2}}
    p1 =  0.5; p2 = -0.5; p3 =  0.5; p4 =  0.5;
    q1 =  0.5; q2 =  0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ7W[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);										
		lineMeshQ7W.vertex(edgeLineQ7W[n]);
		lineMeshQ7W.color(Color(0.0, 1.0, 1.0, 1.0)); // CYAN


//  {{1/2, 1/2, -(1/2), -(1/2)}, {1/2, 1/2, -(1/2), 1/2}}
    p1 =  0.5; p2 =  0.5; p3 = -0.5; p4 = -0.5;
    q1 =  0.5; q2 =  0.5; q3 = -0.5; q4 =  0.5;
		edgeLineQ8Z[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);										
		lineMeshQ8Z.vertex(edgeLineQ8Z[n]);
		lineMeshQ8Z.color(Color(0.3, 0.3, 1.0, 1.0)); // VIOLET
			  
//  {{1/2, 1/2, -(1/2), -(1/2)}, {1/2, 1/2, 1/2, -(1/2)}}
    p1 =  0.5; p2 =  0.5; p3 = -0.5; p4 = -0.5;
    q1 =  0.5; q2 =  0.5; q3 =  0.5; q4 = -0.5;
		edgeLineQ8Y[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);						
		lineMeshQ8Y.vertex(edgeLineQ8Y[n]);
		lineMeshQ8Y.color(Color(1.0, 0.0, 1.0, 1.0)); // MAGENTA
			  
//  {{1/2, 1/2, -(1/2), 1/2}, {1/2, 1/2, 1/2, 1/2}}
    p1 =  0.5; p2 =  0.5; p3 = -0.5; p4 =  0.5;
    q1 =  0.5; q2 =  0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ8X[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);
		lineMeshQ8X.vertex(edgeLineQ8X[n]);
		lineMeshQ8X.color(Color(1.0, 1.0, 0.0, 1.0)); // YELLOW

//  {{1/2, 1/2, 1/2, -(1/2)}, {1/2, 1/2, 1/2, 1/2}}
    p1 =  0.5; p2 =  0.5; p3 =  0.5; p4 = -0.5;
    q1 =  0.5; q2 =  0.5; q3 =  0.5; q4 =  0.5;
		edgeLineQ8W[n] = Vec3f(
			(p2-p2*t+q2*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p3-p3*t+q3*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2))),
			(p4-p4*t+q4*t) / (p1*(-1+t)-q1*t + sqrt(pow((p1-p1*t+q1*t),2)+pow((p2-p2*t+q2*t),2)+pow((p3-p3*t+q3*t),2)+pow((p4-p4*t+q4*t),2)))
			);										
		lineMeshQ8W.vertex(edgeLineQ8W[n]);
		lineMeshQ8W.color(Color(0.3, 1.0, 0.3, 1.0)); // LIME
		
		n++;
		//}
		}
	} // doApp()
	
	virtual ~doApp() {}		// what does this do?

  // ANIMATE
  virtual void onAnimate(double dt) {	  
  }
		
	// DRAW 
  	virtual void onDraw(Graphics& g) {
	  
	  	material(); // material ...
	  	light();    // light ...

		static struct Circle : Mesh {
			Circle() {
				primitive(Graphics::LINE_STRIP);
				for (float r = 0; r < M_2PI; r += 0.01f)
					vertex(cos(r), sin(r), 0);
			}
		} circle;
		
		// SCALE: there is a question of where to break out scale
		// This is the master scale for the graph
		g.pushMatrix();	 
		g.scale(2, 2, 2); // 1 is the original scale
	  // PIXEL MESH 
	if (lines) {
		  
		g.pushMatrix();	 
		g.depthTesting(true);
		g.blending(true);
		g.blendModeTrans();
		g.pointSize(6);
		  
		g.draw(lineMeshQ1Z); g.draw(lineMeshQ1Y); g.draw(lineMeshQ1X); g.draw(lineMeshQ1W);
		g.draw(lineMeshQ2Z); g.draw(lineMeshQ2Y); g.draw(lineMeshQ2X); g.draw(lineMeshQ2W);
		g.draw(lineMeshQ3Z); g.draw(lineMeshQ3Y); g.draw(lineMeshQ3X); g.draw(lineMeshQ3W);
		g.draw(lineMeshQ4Z); g.draw(lineMeshQ4Y); g.draw(lineMeshQ4X); g.draw(lineMeshQ4W);
		g.draw(lineMeshQ5Z); g.draw(lineMeshQ5Y); g.draw(lineMeshQ5X); g.draw(lineMeshQ5W);
		g.draw(lineMeshQ6Z); g.draw(lineMeshQ6Y); g.draw(lineMeshQ6X); g.draw(lineMeshQ6W);
		g.draw(lineMeshQ7Z); g.draw(lineMeshQ7Y); g.draw(lineMeshQ7X); g.draw(lineMeshQ7W);
		g.draw(lineMeshQ8Z); g.draw(lineMeshQ8Y); g.draw(lineMeshQ8X); g.draw(lineMeshQ8W);

		g.popMatrix();
	}
	

// XYZ axes	 
	if (axes_XYZ) {
		  
	g.pushMatrix();	  
	g.lineWidth(3);
	g.scale(sphere_scale, sphere_scale, sphere_scale);
	
	// RED X
		g.color(1, 0, 0);
	  	g.begin(g.LINES); g.vertex( 1, 0, 0); g.vertex( -1, 0, 0); g.end();
	// GREEN Y
		g.color(0, 1, 0);
	  	g.begin(g.LINES); g.vertex( 0, 1, 0); g.vertex( 0, -1, 0); g.end();
	// BLUE Z
		g.color(0, 0, 1);
	  	g.begin(g.LINES); g.vertex( 0, 0, 1); g.vertex( 0, 0, -1); g.end();
	
	g.popMatrix();
		  
	} // if (axes_XYZ)


// AXIS 3D RINGS
	if (axes_3D) {

	g.pushMatrix();	 
	g.lineWidth(4);
	g.scale(sphere_scale, sphere_scale, sphere_scale);
	// XY plane, Z axis (blue)
	  	g.color(0, 0, 1);
	  	g.draw(circle);
	// YZ plane, X axis (red)
	  	g.color(1, 0, 0);
	  	g.rotate (90, 0, 1, 0); 
	  	g.draw(circle);
	// XZ plane, Y axis (green)
	  	g.color(0, 1, 0);
	  	g.rotate (90, 1, 0, 0); 
	  	g.draw(circle);
	g.popMatrix();
		  
	} // if (axes_3D)


	
	  	// TEXT starts here

    	// Depth test so transparent faces are sorted properly
    	g.polygonMode(Graphics::FILL);
    	glAlphaFunc(GL_GREATER, 0.5);
    	glEnable(GL_ALPHA_TEST);
    	shader().uniform("texture", 1.0);

    // For Each Octant

      	const char* text = "null";

      	g.pushMatrix(); // draw octant labels
      	Vec3d xform = Vec3f(0,0,0);
        g.translate(xform);
        g.scale(.01);  
        Vec3d forward = Vec3d(pose.pos() - xform).normalize(); // xform Vec3d with Vec3f constant
        Quatd rot = Quatd::getBillboardRotation(forward, pose.uu()); // uu is up vector
        g.rotate(rot);
        stringstream sstream;

        string temp_str = sstream.str();
        text = temp_str.c_str();
        font1.render(g, text);
      	g.popMatrix(); // end octant lables


    	glDisable(GL_ALPHA_TEST);
    	shader().uniform("texture", 0.0);
		g.popMatrix(); // master draw loop, encapsulates master scale

	} // onDraw
	
	// MESSAGE FROM MAX
	virtual void onMessage(osc::Message& m) {
		std::string param;
		float w, x, y, z;
		int i;

		if (m.addressPattern() == "/as_view_quat") {
			m >> x >> y >> z >> w;
			nav().quat().set(x, y, z, w);
			nav().quat() *= Quatd().fromAxisY(M_PI);
		} else if (m.addressPattern() == "/as_view_pos") {
			m >> x >> y >> z;
			nav().pos().set(x, y, z);
		} else if (m.addressPattern() == "/as_key") {
			m >> i; 
			printf("OSC /as_key %d\n", i);
			if (i == ' ') { /**/ }
		} else { m.print(); } // as_key
	} // onMessage
	
	// KEYBOARD commands local
	virtual bool onKeyDown(const Keyboard& k){
		//keydown(k.key());
		if (k.key() == ' ') { /**/ }
		return true;
	} // onKeyDown
	
  std::string fragmentCode() {
    return AL_STRINGIFY(
      uniform float lighting;
      uniform float texture;
      uniform sampler2D texture0;
      varying vec4 color;
      varying vec3 normal, lightDir, eyeVec;

      void main() {
        vec4 colorMixed;
        vec4 textureColor = texture2D(texture0, gl_TexCoord[0].st);
        if( texture > 0.0){ colorMixed = mix(color, textureColor, texture);} 
        else {colorMixed = color;}
		vec4 final_color = colorMixed * gl_LightSource[0].ambient;
        vec3 N = normalize(normal);
        vec3 L = lightDir;
        float lambertTerm = max(dot(N, L), 0.0);
        final_color += gl_LightSource[0].diffuse * colorMixed * lambertTerm;
        vec3 E = eyeVec;
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(R, E), 0.0), 0.9 + 1e-20);
        final_color += gl_LightSource[0].specular * spec;
        gl_FragColor = mix(colorMixed, final_color, lighting);
		if (texture > 0.0) gl_FragColor.a = textureColor.r; // sets alpha to 0 for background
      }
    );
  } // fragmentCode

}; // struct MyApp : OmniApp

int main(int argc, char* argv[]) {
	doApp().start();
//	START
	return 0;
}
