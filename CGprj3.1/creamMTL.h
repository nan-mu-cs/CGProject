// Created with mtl2opengl.pl

/*
source files: /Users/Apple/Desktop/cream/cream.obj, /Users/Apple/Desktop/cream/cream.mtl
materials: 3

Name: lambert4SG
Ka: 0.000, 0.000, 0.000
Kd: 0.270, 0.070, 0.050
Ks: 0, 0, 0
Ns: 1

Name: lambert8SG
Ka: 0.060, 0.060, 0.060
Kd: 1.000, 1.000, 1.000
Ks: 0, 0, 0
Ns: 1

Name: lambert9SG
Ka: 0.000, 0.000, 0.000
Kd: 1.180, 0.880, 0.510
Ks: 0, 0, 0
Ns: 1

*/


int creamMTLNumMaterials = 3;

int creamMTLFirst [3] = {
0,
720,
3666,
};

int creamMTLCount [3] = {
720,
2946,
1566,
};

float creamMTLAmbient [3][3] = {
0.000,0.000,0.000,
0.060,0.060,0.060,
0.000,0.000,0.000,
};

float creamMTLDiffuse [3][3] = {
0.270,0.070,0.050,
1.000,1.000,1.000,
1.180,0.880,0.510,
};

float creamMTLSpecular [3][3] = {
0,0,0,
0,0,0,
0,0,0,
};

float creamMTLExponent [3] = {
1,
1,
1,
};

